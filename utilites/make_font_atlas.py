# This script generates a simple font atlas (grayscale alpha) and JSON metadata using freetype-py and Pillow.
# It is intended as a starting point. It will render specified glyphs at a fixed pixel size,
# pack them into a single atlas image using a simple shelf packing algorithm, and write out metadata.
#
# Usage (example):
#   python make_font_atlas.py /path/to/font.ttf 48 ascii.txt output_atlas.png output_meta.json
#
# The script will detect missing dependencies and print instructions instead of crashing.
# If you want to run inside this environment, make sure freetype-py and Pillow are installed:
#   pip install freetype-py Pillow


import sys
import json
import math
import os
import freetype

from argparse import ArgumentParser
from PIL import Image


def load_glyph_bitmap(face, char, pixel_size):
    """Load and render one glyph; returns (PIL Image grayscale, metrics dict)"""
    face.set_pixel_sizes(0, pixel_size)
    face.load_char(char, freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_NORMAL)
    glyph = face.glyph
    bitmap = glyph.bitmap
    width, rows = bitmap.width, bitmap.rows
    buffer = bitmap.buffer
    # Create a grayscale (L) image from buffer. freetype bitmap.buffer is a bytes-like row-major
    img = Image.new("L", (width, rows), color=0)

    if width > 0 and rows > 0:
        hex_string = ""
        for byte in buffer:
            hex_string += f"{byte:02x}"
        byte_array = bytes.fromhex(hex_string)
        # print(buffer)
        img.frombytes(byte_array)

    metrics = {
        "width": width,
        "height": rows,
        "left": glyph.bitmap_left,
        "top": glyph.bitmap_top,
        "advance": int(glyph.advance.x >> 6)
    }

    return img, metrics

def simple_shelf_pack(sizes, atlas_width, padding=1):
    """
    Simple shelf packer.
    sizes: list of (w,h, id) tuples
    returns dict id -> (x,y,w,h) and atlas_height
    """
    x = padding
    y = padding
    shelf_h = 0
    placements = {}
    for w,h,ident in sizes:
        if x + w + padding > atlas_width:
            # new shelf
            y += shelf_h + padding
            x = padding
            shelf_h = 0
        placements[ident] = (x, y, w, h)
        x += w + padding
        shelf_h = max(shelf_h, h)
    atlas_height = y + shelf_h + padding
    return placements, atlas_height

def build_atlas(font_path, pixel_size, chars,
                atlas_width=1024, height_compensation=True,
                padding=1, out_image="atlas.png", out_meta="atlas.json",
                pretty=False):
    face = freetype.Face(font_path)
    glyph_images = {}
    glyph_metrics = {}
    sizes = []
    print(f"Rendering {len(chars)} glyphs at size {pixel_size}px...")
    for ch in chars:
        img, metrics = load_glyph_bitmap(face, ch, pixel_size)
        codepoint = ord(ch)
        ident = str(codepoint)
        glyph_images[ident] = img
        glyph_metrics[ident] = metrics
        sizes.append((img.width, img.height, ident))

    # Simple packing
    placements, atlas_height = simple_shelf_pack(sizes, atlas_width, padding=padding)

    if height_compensation and atlas_height < atlas_width:
        atlas_height = atlas_width

    print(f"Packing into atlas {atlas_width}x{atlas_height}...")

    # Create RGBA atlas. We'll put glyph grayscale into alpha channel; RGB = 255 (white)
    atlas = Image.new("RGBA", (atlas_width, atlas_height), (255,255,255,0))

    atlas_info = {}
    atlas_info["texture"] = { "width": atlas_width, "height": atlas_height }
    atlas_info["metadata"] = {
        "font": os.path.basename(font_path),
        "format": "RGBA",
        "textureFileName": os.path.basename(out_image),
        "size": [atlas_width, atlas_height],
    }

    frames = {}

    # metadata = {
    #     "font": os.path.basename(font_path),
    #     "pixel_size": pixel_size,
    #     "atlas_width": atlas_width,
    #     "atlas_height": atlas_height,
    #     "padding": padding,
    #     "glyphs": {}
    # }

    for ident, (x,y,w,h) in placements.items():
        img = glyph_images[ident]
        # create RGBA image where alpha is glyph grayscale and RGB is white
        rgba = Image.new("RGBA", img.size, (255,255,255,0))
        if img.width > 0 and img.height > 0:
            rgba.putalpha(img)
        atlas.paste(rgba, (x,y), rgba)
        m = glyph_metrics[ident]
        # metadata["glyphs"][ident] = {
        #     "char": chr(int(ident)),
        #     "codepoint": int(ident),
        #     "x": x,
        #     "y": y,
        #     "w": w,
        #     "h": h,
        #     "left": m["left"],
        #     "top": m["top"],
        #     "advance": m["advance"]
        # }

        frames[chr(int(ident))] = {
            "frame": [x, y, w, h],
            "rotated": False,
            "sourceSize": [w, h],
        }
    
    atlas_info["frames"] = frames

    atlas.save(out_image)
    with open(out_meta, "w", encoding="utf-8") as f:
        json.dump(atlas_info, f, ensure_ascii=False, indent=2 if pretty else None)

    print(f"Saved atlas: {out_image}")
    print(f"Saved metadata: {out_meta}")
    return out_image, out_meta


def load_char_files(path):
    if args.chars_file.lower() == "ascii":
        return [chr(c) for c in range(32, 127)]

    # read chars from file (one line may contain characters)
    if not os.path.exists(args.chars_file):
        print(f"Chars file not found: {args.chars_file}")
        sys.exit(1)

    with open(args.chars_file, "r", encoding="utf-8") as f:
        data = f.read()
        # unique preserve order
        chars = set()
        for ch in data:
            chars.add(ch)

        return list(chars)


if __name__ == "__main__":
    parser = ArgumentParser(description="Generate a font atlas from a TTF font file.")

    parser.add_argument("font", help="Path to the TTF font file.")
    parser.add_argument("-p", "--pixel_size", type=int, default=36, help="Pixel size for the font.")
    parser.add_argument("-c", "--chars_file", type=str, default="ascii", help="Path to the file containing characters or 'ascii'.")
    parser.add_argument("-o", "--out", type=str, default="font_atlas", help="Path to the output name file.")
    parser.add_argument("--pretty", action="store_true", help="Pretty-print JSON output.")

    parser.add_argument("-w", "--atlas_width", type=int, default=256, help="Width of the font atlas.")

    args = parser.parse_args()

    out_image = args.out + ".png"
    out_meta = args.out + ".json"
    chars = load_char_files(args.chars_file)
    build_atlas(args.font, args.pixel_size, chars, atlas_width=args.atlas_width, out_image=out_image, out_meta=out_meta, pretty=args.pretty)

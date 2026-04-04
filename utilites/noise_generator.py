import os
import numpy

from PIL import Image
from noise import pnoise2

FILE_PATH = os.path.abspath(__file__)
OUTPUT_FOLDER = os.path.join(os.path.dirname(FILE_PATH), "outputs")


def generate_perlin_noise(size, zoom_scale=100.0, octaves=6, persistence=0.5, lacunarity=2.0, base=0):
    # Generate noise values
    noise_data = numpy.zeros((size, size), dtype=numpy.float32)

    for y in range(size):
        for x in range(size):
            nx = x / zoom_scale
            ny = y / zoom_scale
            noise_value = pnoise2(nx, ny,
                                  octaves=octaves,
                                  persistence=persistence,
                                  lacunarity=lacunarity,
                                  repeatx=size / 2,
                                  repeaty=size / 2,
                                  base=base)
            # Map from [-1, 1] to [0, 1]
            noise_data[y][x] = (noise_value + 1) / 2.0

    # Convert to image
    img = Image.fromarray(numpy.uint8(noise_data * 255), mode='L')
    img.save(os.path.join(OUTPUT_FOLDER, f"perlin_noise_{size}.jpg"))

# Settings
width, height = 256, 256
grid_size = 16  # how many cells (lower = bigger noise blobs)

def fade(t):
    # Smoothstep-like interpolation (Perlin's fade function)
    return 6*t**5 - 15*t**4 + 10*t**3

def lerp(a, b, t):
    return a + t * (b - a)

# Generate random 2D gradient vectors at grid points
def random_gradients(grid_w, grid_h):
    angles = 2 * numpy.pi * numpy.random.rand(grid_h + 1, grid_w + 1)
    gradients = numpy.dstack((numpy.cos(angles), numpy.sin(angles)))
    return gradients

def dot_grid_gradient(ix, iy, x, y, gradients):
    dx = x - ix
    dy = y - iy
    gradient = gradients[iy, ix]
    return dx * gradient[0] + dy * gradient[1]

def gradient_noise(width, height, grid_size):
    grid_w = width // grid_size
    grid_h = height // grid_size
    gradients = random_gradients(grid_w, grid_h)

    noise = numpy.zeros((height, width))

    for y in range(height):
        for x in range(width):
            # Position within grid cell
            xf = x / grid_size
            yf = y / grid_size
            x0 = int(xf)
            y0 = int(yf)
            x1 = x0 + 1
            y1 = y0 + 1

            sx = fade(xf - x0)
            sy = fade(yf - y0)

            # Dot products at corners
            n0 = dot_grid_gradient(x0, y0, xf, yf, gradients)
            n1 = dot_grid_gradient(x1, y0, xf, yf, gradients)
            ix0 = lerp(n0, n1, sx)

            n2 = dot_grid_gradient(x0, y1, xf, yf, gradients)
            n3 = dot_grid_gradient(x1, y1, xf, yf, gradients)
            ix1 = lerp(n2, n3, sx)

            value = lerp(ix0, ix1, sy)
            noise[y, x] = value

    # Normalize to [0, 255]
    noise -= noise.min()
    noise /= noise.max()
    return (noise * 255).astype(numpy.uint8)


if __name__ == "__main__":
    if not os.path.exists(OUTPUT_FOLDER):
        os.makedirs(OUTPUT_FOLDER)

    generate_perlin_noise(512)
    generate_perlin_noise(1024)
    generate_perlin_noise(2048)

    # Generate and save
    noise_img = Image.fromarray(gradient_noise(width, height, grid_size), mode='L')
    noise_img.save(os.path.join(OUTPUT_FOLDER, "gradient_noise.jpg"))

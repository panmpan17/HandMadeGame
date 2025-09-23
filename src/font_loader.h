#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include "debug_macro.h"


class FontLoader {
public:
    inline static FontLoader* getInstance() { return ins; }

    inline static void Initialize() { if (!ins) ins = new FontLoader(); }

    FontLoader()
    {
        if (FT_Init_FreeType(&m_ftLibrary))
        {
            m_ftLibrary = nullptr;
            LOGERRLN("Failed to initialize FreeType library");
            return;
        }

        m_bInitialized = true;
    }
    ~FontLoader()
    {}

    inline void loadFont(const std::string_view& strPath)
    {
        if (!m_bInitialized) return;

        if (FT_New_Face(m_ftLibrary, strPath.data(), 0, &m_ftFace))
        {
            LOGERRLN_EX("Failed to create FreeType face for path: {}", strPath);
            return;
        }

        LOGLN_EX("Loaded font from path: {}", strPath);

        if (FT_Set_Char_Size(m_ftFace,
                             0, // 0 means same as height
                             16 * 64, // 16pt in 1/64th of points
                             300, 300) // Set char size to 16pt at 300x300 dpi
            )
        {
            LOGERRLN_EX("Failed to set char size for font: {}", strPath);
            FT_Done_Face(m_ftFace);
            return;
        }

        // Set pixel size instead of char size
        // if (FT_Set_Pixel_Sizes(m_ftFace,
        //                      0, // 0 means same as height
        //                      16 // 16 pixels high
        //                      )
        //     )
        // {
        //     LOGERRLN_EX("Failed to set pixel size for font: {}", strPath);
        //     FT_Done_Face(m_ftFace);
        //     return;
        // }
    }

    inline void test(char c)
    {
        if (!m_bInitialized || !m_ftFace) return;

        FT_UInt glyphIndex = FT_Get_Char_Index(m_ftFace, c);

        if (glyphIndex == 0)
        {
            LOGERRLN_EX("Character '{}' not found in font", c);
            return;
        }

        if (FT_Load_Glyph(m_ftFace, glyphIndex, FT_LOAD_DEFAULT))
        {
            LOGERRLN_EX("Failed to load glyph for character '{}'", c);
            return;
        }

        // if (FT_Render_Glyph(m_ftFace->glyph, FT_RENDER_MODE_NORMAL))
        // {
        //     LOGERRLN_EX("Failed to render glyph for character '{}'", c);
        //     return;
        // }
    }


private:
    static FontLoader* ins;

    bool m_bInitialized = false;
    FT_Library m_ftLibrary = nullptr;

    bool m_bFontLoaded = false;
    FT_Face m_ftFace = nullptr;
};

FontLoader* FontLoader::ins = nullptr;
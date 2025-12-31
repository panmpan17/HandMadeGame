#pragma once

#include <unordered_map>
#include <linmath.h>
#include <ft2build.h>
#include FT_FREETYPE_H


struct Character
{
    unsigned int m_nTextureID;
    vec2 m_vecSize;
    vec2 m_vecBearing;
    unsigned int m_nAdvance;
};


class Font
{
public:
    friend class FontLoader;

    ~Font()
    {
        unloadFont();
    }

    inline void unloadFont()
    {
        if (m_ftFace)
        {
            FT_Done_Face(m_ftFace);
            m_ftFace = nullptr;
        }

        // Delete character textures
        for (auto& pair : m_mapCharacters)
        {
            glDeleteTextures(1, &pair.second.m_nTextureID);
        }
        m_mapCharacters.clear();
    }

    inline void unloadFontFace()
    {
        if (m_ftFace)
        {
            FT_Done_Face(m_ftFace);
            m_ftFace = nullptr;
        }
    }

private:
    FT_Face m_ftFace = nullptr;

    std::unordered_map<char, Character> m_mapCharacters;

    inline void setCharSize(FT_F26Dot6 charHeight, FT_UInt dpi)
    {
        if (m_ftFace)
        {
            if (FT_Set_Char_Size(m_ftFace,
                                 0, // 0 means same as height
                                 charHeight * 64, // in 1/64th of points
                                 dpi, dpi) // Set char size at dpi x dpi
                )
            {
                LOGERR("Failed to set char size for font face");
                return;
            }
        }
    }

    inline void setPixelSize(FT_UInt pixelHeight)
    {
        if (m_ftFace)
        {
            if (FT_Set_Pixel_Sizes(m_ftFace,
                                  0, // 0 means same as height
                                  pixelHeight // in pixels
                                  )
                )
            {
                LOGERR("Failed to set pixel size for font face");
                return;
            }
        }
    }

    inline void loadAsciiCharacters()
    {
        if (!m_ftFace) return;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph
            if (FT_Load_Char(m_ftFace, c, FT_LOAD_RENDER))
            {
                LOGERR("Failed to load Glyph for char: {}", c);
                continue;
            }

            // Generate texture
            unsigned int nTexture;
            glGenTextures(1, &nTexture);
            glBindTexture(GL_TEXTURE_2D, nTexture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                m_ftFace->glyph->bitmap.width,
                m_ftFace->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                m_ftFace->glyph->bitmap.buffer
            );

            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Now store character for later use
            Character character = {
                nTexture,
                { (float)m_ftFace->glyph->bitmap.width, (float)m_ftFace->glyph->bitmap.rows },
                { (float)m_ftFace->glyph->bitmap_left, (float)m_ftFace->glyph->bitmap_top },
                (unsigned int)m_ftFace->glyph->advance.x
            };
            m_mapCharacters.insert(std::pair<char, Character>(c, character));
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }
};


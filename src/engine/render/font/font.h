#pragma once

#include <glad/gl.h>
#include <unordered_map>
#include <utility>
#include <linmath.h>
#include <ft2build.h>
#include "../core/renderer.h"
#include "../../core/debug_macro.h"

#include FT_FREETYPE_H

#define FORMAT_CHAR16(c) static_cast<std::uint_least16_t>(c)

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

    Font() = default;
    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    Font(Font&& other) noexcept
    {
        moveFrom(std::move(other));
    }

    Font& operator=(Font&& other) noexcept
    {
        if (this != &other)
        {
            unloadFont();
            moveFrom(std::move(other));
        }
        return *this;
    }

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
            if (Renderer::isUsingOpenGL())
            {
                glDeleteTextures(1, &pair.second.m_nTextureID);
            }
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

    Character* getCharacter(char16_t c)
    {
        auto it = m_mapCharacters.find(c);
        if (it != m_mapCharacters.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    // Returns true if character was loaded successfully, false if it failed to load
    inline bool loadCharacterTexture(char16_t c)
    {
        LOGLN("Loading character: {}", FORMAT_CHAR16(c));
        std::unordered_map<char16_t, Character>::iterator it = m_mapCharacters.find(c);
        if (it != m_mapCharacters.end())
        {
            return true;
        }

        if (!m_ftFace)
        {
            LOGERR("Font face not loaded, cannot load character: {}", FORMAT_CHAR16(c));
            return false;
        }

        if (FT_Load_Char(m_ftFace, c, FT_LOAD_RENDER))
        {
            LOGERR("Failed to load Glyph for char: {}", FORMAT_CHAR16(c));
            return false;
        }

        if (Renderer::isUsingOpenGL())
        {
            unsigned int nTexture;
            glGenTextures(1, &nTexture);
            glBindTexture(GL_TEXTURE_2D, nTexture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

            // Now store character for later use
            Character character = {
                nTexture,
                { (float)m_ftFace->glyph->bitmap.width, (float)m_ftFace->glyph->bitmap.rows },
                { (float)m_ftFace->glyph->bitmap_left, (float)m_ftFace->glyph->bitmap_top },
                (unsigned int)m_ftFace->glyph->advance.x
            };
            m_mapCharacters.insert(std::pair<char16_t, Character>(c, character));
            glBindTexture(GL_TEXTURE_2D, 0);
            return true;
        }

        return false;
    }

private:
    FT_Face m_ftFace = nullptr;

    std::unordered_map<char16_t, Character> m_mapCharacters;

    inline void moveFrom(Font&& other) noexcept
    {
        m_ftFace = other.m_ftFace;
        m_mapCharacters = std::move(other.m_mapCharacters);
        other.m_ftFace = nullptr;
        other.m_mapCharacters.clear();
    }

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

        if (Renderer::isUsingOpenGL())
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
        }

        for (char16_t c = 0; c < 128; c++)
        {
            loadCharacterTexture(c);
        }
    }
};


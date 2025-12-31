#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include "font.h"
#include "../../core/debug_macro.h"


class FontLoader {
public:
    inline static FontLoader* getInstance() { return ins; }

    inline static void Initialize() { if (!ins) ins = new FontLoader(); }

    FontLoader()
    {
        if (FT_Init_FreeType(&m_ftLibrary))
        {
            m_ftLibrary = nullptr;
            // TODO: Handle error
            LOGERR("Failed to initialize FreeType library");
            return;
        }

        m_bInitialized = true;
    }
    ~FontLoader()
    {
        if (m_ftLibrary)
        {
            FT_Done_FreeType(m_ftLibrary);
            m_ftLibrary = nullptr;
        }
        m_bInitialized = false;
    }

    inline Font* loadFont(const std::string_view& strPath)
    {
        if (!m_bInitialized) return nullptr;

        Font oFont;
        if (FT_New_Face(m_ftLibrary, strPath.data(), 0, &oFont.m_ftFace))
        {
            LOGERR("Failed to create FreeType face for path: {}", strPath);
            return nullptr;
        }

        oFont.setCharSize(16, 300);
        // oFont.setPixelSize(16);

        oFont.loadAsciiCharacters();
        oFont.unloadFontFace();

        m_mapLoadedFonts.emplace(strPath, std::move(oFont));

        return &m_mapLoadedFonts.at(strPath);
    }

    inline Font* getFont(const std::string_view& strPath)
    {
        auto it = m_mapLoadedFonts.find(strPath);
        if (it != m_mapLoadedFonts.end())
        {
            return &it->second;
        }
        return loadFont(strPath);
    }

private:
    static FontLoader* ins;

    bool m_bInitialized = false;
    FT_Library m_ftLibrary = nullptr;

    std::unordered_map<std::string_view, Font> m_mapLoadedFonts;
};

FontLoader* FontLoader::ins = nullptr;
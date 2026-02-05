#pragma once

#include <ft2build.h>
#include <string>
#include <utility>
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

    inline Font* loadFont(std::string strPath)
    {
        if (!m_bInitialized) return nullptr;

        Font oFont;
        if (FT_New_Face(m_ftLibrary, strPath.c_str(), 0, &oFont.m_ftFace))
        {
            LOGERR("Failed to create FreeType face for path: {}", strPath);
            return nullptr;
        }

        oFont.setCharSize(16, 300);
        // oFont.setPixelSize(16);

        oFont.loadAsciiCharacters();
        oFont.unloadFontFace();

        auto [it, _] = m_mapLoadedFonts.emplace(std::move(strPath), std::move(oFont));

        return &it->second;
    }

    inline Font* getFont(std::string_view strPath)
    {
        std::string ownedPath(strPath);
        auto it = m_mapLoadedFonts.find(ownedPath);
        if (it != m_mapLoadedFonts.end())
        {
            return &it->second;
        }
        return loadFont(std::move(ownedPath));
    }

private:
    static inline FontLoader* ins = nullptr;

    bool m_bInitialized = false;
    FT_Library m_ftLibrary = nullptr;

    std::unordered_map<std::string, Font> m_mapLoadedFonts;
};

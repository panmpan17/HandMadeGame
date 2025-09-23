#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H


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
            return;
        }

        m_bInitialized = true;
    }
    ~FontLoader()
    {}

private:
    static FontLoader* ins;

    FT_Library m_ftLibrary;
    bool m_bInitialized = false;
};

FontLoader* FontLoader::ins = nullptr;
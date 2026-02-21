#include "stdafx.h"
#include "steam_api.h"

#include "../utils/filesystem.h"
#include "../utils/file_utils.h"
#include "../engine/core/debug_macro.h"

class SteamHook
{
public:
    SteamHook()
    {
#if IS_DEBUG_VERSION
        
        m_bShouldInitializeSteamAPI = fs::exists(FileUtils::getExecutableDirectory() + "/steam_appid.txt");
#else
        m_bShouldInitializeSteamAPI = true;
#endif
    }

    bool init()
    {
        if (!m_bShouldInitializeSteamAPI)
        {
            LOGLN("Skipping Steam API initialization.");
            return true;
        }

        LOGLN("SteamHook constructor called");
        if ( SteamAPI_RestartAppIfNecessary( k_uAppIdInvalid ) )
        {
            return false;
        }
        if (!Steamworks_InitCEGLibrary()) // What does this do?
        {
            std::cerr << "Failed to initialize Steamworks CEGLibrary." << std::endl;
            return false;
        }

        SteamErrMsg errMsg = { 0 };
        if ( SteamAPI_InitEx( &errMsg ) != k_ESteamAPIInitResult_OK )
        {
            std::cerr << "SteamAPI_Init() failed: " << errMsg << std::endl;
            return false;
        }

        m_bIsSteamAPIInitialized = true;
        return true;
    }

    void shutdown()
    {
        if (m_bIsSteamAPIInitialized)
        {
            SteamAPI_Shutdown();
            Steamworks_TermCEGLibrary();
            m_bIsSteamAPIInitialized = false;
        }
    }

private:
    bool m_bShouldInitializeSteamAPI = false;
    bool m_bIsSteamAPIInitialized = false;
};
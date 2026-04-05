#include "engine/core/window.h"
#include "engine/core/error_handler.h"
#include "engine/core/debug_macro.h"

#include "game/testing/test.h"
// #include "game/pingpong/game.h"
// #include "game/colorpicker/picker.h"
#include "game/testing/minimal_metal_test.h"
#include "game/desktop_farm/desktop_farm.h"

#include "utils/file_utils.h"

#ifdef INCLUDE_STEAMWORKS
#include "steam/steam_hook.h"
#endif

#include <soloud.h>
#include <soloud_wav.h>


int main(int nArgumentCount, char* arrArguments[])
{

    SoLoud::Soloud soloud; // Engine core
    SoLoud::Wav sample;    // Audio source

    const auto audioInitResult = soloud.init();
    const bool bAudioInitialized = audioInitResult == SoLoud::SO_NO_ERROR;

    if (!bAudioInitialized)
    {
        std::cerr << "Failed to initialize SoLoud: "
                  << soloud.getErrorString(audioInitResult) << std::endl;
    }
    else
    {
        const std::string strSoundPath = "/Users/panmichael/Projects/GLFWTest/assets/camera_zoom.wav";
        const auto loadResult = sample.load(strSoundPath.c_str());

        if (loadResult != SoLoud::SO_NO_ERROR)
        {
            std::cerr << "Failed to load sound: "
                      << soloud.getErrorString(loadResult)
                      << " (" << strSoundPath << ')' << std::endl;
        }
        else
        {
            soloud.play(sample);
        }
    }

#ifdef INCLUDE_STEAMWORKS
    SteamHook steamHook;
    if (!steamHook.init())
    {
        std::cerr << "Failed to initialize Steam API." << std::endl;
        return -1;
    }
#endif

#if IS_DEBUG_VERSION
    if (nArgumentCount >= 2)
    {
        LOGLN("Setting resource path to: {}", arrArguments[1]);
        FileUtils::setResourcesPath(arrArguments[1]);
    }
#endif

    registerSignalHandlers();
    // ColorPicker colorPicker;
    DesktopFarmGame desktopFarmGame;

    do
    {
        Window window;
        window.setResizable(true);
        // window.setTransparentBackground(true);
        // colorPicker.preconfigureWindowObject(&window);
        desktopFarmGame.preconfigureWindowObject(&window);
        if (!window.configureAndCreateWindow())
        {
            return -1; // Initialization failed
        }

        window.setupManagers();
        window.setupGameEngineRelatedObject();

        PROFILER_START_TIMER();
        // setupPostProcess();
        // PROFILER_END_TIMER("World", "Setup post process");
        // createDemo1();
        // createVisualEffectDemo();
        // createLightingShadowDemo();
        // createPingPongGame();
        // sceneTest();
        // createProfolioSceneDemo();
        // colorPicker.pickerMain();
        // serializationTest();
        // firstTriangeTest();
        // fullTest();
        desktopFarmGame.setupWorldScene();
        PROFILER_END_TIMER("World", "Init");

        window.mainLoop();
    }
#if IS_DEBUG_VERSION
    while (Window::sm_bRestartRequested);
#else
    while (false);
#endif

#ifdef INCLUDE_STEAMWORKS
    steamHook.shutdown();
#endif

    if (bAudioInitialized)
    {
        soloud.deinit();
    }

    return 0;
}

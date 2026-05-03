#include "engine/core/window.h"
#include "engine/core/error_handler.h"
#include "engine/core/debug_macro.h"
#include "engine/core/audio/audio_engine.h"
#include "engine/core/audio/audio_clip.h"

#include "game/testing/test.h"
// #include "game/pingpong/game.h"
// #include "game/colorpicker/picker.h"
#include "game/testing/minimal_metal_test.h"
#include "game/desktop_farm/desktop_farm.h"

#include "utils/file_utils.h"

#ifdef INCLUDE_STEAMWORKS
#include "steam/steam_hook.h"
#endif


int main(int nArgumentCount, char* arrArguments[])
{
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
    DesktopFarmGame* desktopFarmGame = nullptr;
    // desktopFarmGame = new DesktopFarmGame();

    do
    {
        Window window;
        window.setResizable(true);
        // window.setTransparentBackground(true);
        // colorPicker.preconfigureWindowObject(&window);
        if (desktopFarmGame)
        {
            desktopFarmGame->preconfigureWindowObject(&window);
        }
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
        fullTest();
        if (desktopFarmGame)
        {
            desktopFarmGame->setupWorldScene();
        }
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

    return 0;
}

#include "engine/core/window.h"
#include "engine/core/error_handler.h"
#include "engine/core/debug_macro.h"

#include "game/testing/test.h"
// #include "game/pingpong/game.h"
// #include "game/colorpicker/picker.h"
#include "game/testing/minimal_metal_test.h"
#include "game/desktop_farm/desktop_farm.h"

#include "utils/file_utils.h"


int main(int nArgumentCount, char* arrArguments[])
{
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
    return 0;
}

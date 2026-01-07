#include "engine/core/window.h"
#include "engine/core/error_handler.h"
#include "engine/core/debug_macro.h"

// #include "game/testing/test.h"
// #include "game/pingpong/game.h"
// #include "game/colorpicker/picker.h"
#include "game/testing/minimal_metal_test.h"

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

    Window window;
    // ColorPicker colorPicker;

    try
    {
        window.setResizable(true);
        // colorPicker.preconfigureWindowObject(&window);
        if (!window.configureAndCreateWindow())
        {
            return -1; // Initialization failed
        }

        window.initializeGraphicsAPI();
        window.setupManagers();
        window.setupGameEngineRelatedObject();

        PROFILER_START_TIMER()
        // setupPostProcess();
        PROFILER_END_TIMER("World", "Setup post process");
        // createDemo1();
        // createVisualEffectDemo();
        // createLightingShadowDemo();
        // createPingPongGame();
        // sceneTest();
        // createProfolioSceneDemo();
        // colorPicker.pickerMain();
        // serializationTest();
        firstTriangeTest();
        PROFILER_END_TIMER("World", "Init")

        window.mainLoop();
    }
    catch (const std::exception& e)
    {
        std::cerr << "main.cpp catch exception: " << e.what() << '\n';
        return -1;
    }

    return 0;
}

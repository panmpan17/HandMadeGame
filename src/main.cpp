#include "engine/core/window.h"
#include "engine/core/error_handler.h"

#include "game/testing/test.h"
#include "game/pingpong/game.h"
#include "game/colorpicker/picker.h"

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

        window.setupManagers();
        window.setupGameEngineRelatedObject();

        setupPostProcess();
        // createDemo1();
        // testShaderMaterial();
        // createVisualEffectDemo();
        createLightingShadowDemo();
        // createPingPongGame();
        // sceneTest();
        // colorPicker.pickerMain();

        window.mainLoop();
    }
    catch (const std::exception& e)
    {
        std::cerr << "main.cpp catch exception: " << e.what() << '\n';
        return -1;
    }

    return 0;
}

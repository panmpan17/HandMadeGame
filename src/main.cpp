#include "engine/core/window.h"
#include "engine/core/error_handler.h"

#include "game/testing/test.h"
#include "game/pingpong/game.h"

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

    try
    {
        window.setResizable(true);
        if (!window.configureAndCreateWindow())
        {
            return -1; // Initialization failed
        }

        window.setupManagers();

        // createDemo1();
        // createVisualEffectDemo();
        createLightingShadowDemo();
        // createPingPongGame();

        window.mainLoop();
    }
    catch (const std::exception& e)
    {
        std::cerr << "main.cpp catch exception: " << e.what() << '\n';
        return -1;
    }

    return 0;
}

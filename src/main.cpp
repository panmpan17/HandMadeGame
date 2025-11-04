#include "engine/core/window.h"
#include "engine/core/error_handler.h"

int main() {
    registerSignalHandlers();

    Window window;

    try
    {
        window.setResizable(true);
        window.configureAndCreateWindow();

        if (!window.isValid())
        {
            return -1; // Initialization failed
        }

        window.start();
    }
    catch (const std::exception& e)
    {
        std::cerr << 20 << e.what() << '\n';
        return -1;
    }

    return 0;
}

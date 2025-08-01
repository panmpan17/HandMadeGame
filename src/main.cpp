#include "window.h"

int main() {
    Window window;
    window.setResizable(true);
    window.configureAndCreateWindow();

    if (!window.isValid())
    {
        return -1; // Initialization failed
    }

    window.start();
    return 0;
}

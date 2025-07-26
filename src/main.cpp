#include "window.h"

int main() {
    Window window;
    if (!window.isValid())
    {
        return -1; // Initialization failed
    }

    window.start();
    return 0;
}

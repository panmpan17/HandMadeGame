#include <GLFW/glfw3.h>
#include <iostream>
#include <print>


#define IS_DEBUG_VERSION 1
#ifdef IS_DEBUG_VERSION
#define DEBUG_PRINT(msg, ...) std::println(msg, __VA_ARGS__)
#else
#define DEBUG_PRINT(msg, ...) do {} while (0)
#endif


class Window {
public:
    Window() {
        if (!glfwInit()) {
            DEBUG_PRINT("Failed to initialize GLFW");
            return;
        }

        m_pWindow = glfwCreateWindow(800, 600, "My GLFW Window", NULL, NULL);
        if (!m_pWindow) {
            DEBUG_PRINT("Failed to create GLFW window");
            glfwTerminate();
            return;
        }
    }

    ~Window() {
        if (m_pWindow) {
            glfwDestroyWindow(m_pWindow);
        }
        glfwTerminate();
    }

    bool isValid() const {
        return m_pWindow != nullptr;
    }

    void start() {
        glfwMakeContextCurrent(m_pWindow);

        while (!glfwWindowShouldClose(m_pWindow)) {
            glfwSwapBuffers(m_pWindow);
            glfwPollEvents();
        }

        glfwTerminate();
    }

private:
    GLFWwindow* m_pWindow = nullptr;
};

int main() {
    Window window;
    if (!window.isValid()) {
        return -1; // Initialization failed
    }

    window.start();
    return 0;
}

// g++ main.cpp -o my_glfw_app -I./include -L./lib -lglfw.3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -Wl,-rpath,@loader_path/lib
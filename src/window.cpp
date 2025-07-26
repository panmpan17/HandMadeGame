#include <GLFW/glfw3.h>
#include "window.h"
#include "debug_macro.h"

Window::Window()
{
    if (!glfwInit())
    {
        DEBUG_PRINT("Failed to initialize GLFW");
        return;
    }

    m_pWindow = glfwCreateWindow(800, 600, "My GLFW Window", NULL, NULL);
    if (!m_pWindow)
    {
        DEBUG_PRINT("Failed to create GLFW window");
        glfwTerminate();
        return;
    }
}

Window::~Window()
{
    if (m_pWindow)
    {
        glfwDestroyWindow(m_pWindow);
    }
    glfwTerminate();
}

void Window::start()
{
    glfwMakeContextCurrent(m_pWindow);

    while (!glfwWindowShouldClose(m_pWindow))
    {
        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }

    glfwTerminate();
}
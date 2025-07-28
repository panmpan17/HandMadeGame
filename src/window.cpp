// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <linmath.h>

#include "window.h"
#include "debug_macro.h"

typedef struct Vertex
{
    vec2 pos;
    vec3 col;
} Vertex;
 
static const Vertex vertices[3] =
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
};

Window::Window()
{
    if (!glfwInit())
    {
        DEBUG_PRINT("Failed to initialize GLFW");
        return;
    }

    configureAndCreateWindow();
}

Window::~Window()
{
    if (m_pWindow)
    {
        glfwDestroyWindow(m_pWindow);
    }
    glfwTerminate();
}

void Window::configureAndCreateWindow()
{
    // Configure GL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_pWindow = glfwCreateWindow(800, 600, "My GLFW Window", NULL, NULL);
    if (!m_pWindow)
    {
        DEBUG_PRINT("Failed to create GLFW window");
        glfwTerminate();
        return;
    }
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
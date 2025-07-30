// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <linmath.h>
#include <functional>

#include "window.h"
#include "camera.h"
#include "draw/shader.h"
#include "draw/quad.h"
#include "draw/triangle.h"
#include "debug_macro.h"
#include "input_handle.h"


Window* Window::ins = nullptr;

Window::Window()
{
    ins = this;

    if (!glfwInit())
    {
        LOGLN("Failed to initialize GLFW");
        return;
    }

    configureAndCreateWindow();
}

Window::~Window()
{
    if (m_pCamera)
    {
        delete m_pCamera;
        m_pCamera = nullptr;
    }
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

    m_pWindow = glfwCreateWindow(m_nWidth, m_nHeight, "My GLFW Window", NULL, NULL);
    if (!m_pWindow)
    {
        LOGLN("Failed to create GLFW window");
        glfwTerminate();
        return;
    }
}

void Window::start()
{
    m_pCamera = new Camera();

    glfwSetKeyCallback(m_pWindow, &InputManager::onKeyCallback);
    glfwSetCursorEnterCallback(m_pWindow, &InputManager::onMouseEnterCallback);
    glfwSetCursorPosCallback(m_pWindow, &InputManager::onMousePosCallback);
    glfwSetMouseButtonCallback(m_pWindow, &InputManager::onMouseButtonCallback);

    // glfwSetWindowOpacity(m_pWindow, 0.5f); // Fun

    glfwMakeContextCurrent(m_pWindow);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1); // Enable vsync
    
    setupGLVertex();
    setupShaders();
    // The order of execute is important here, but not sure why
    // m_pTriangle->setShader(m_pBaseShader);

    if (m_pQuad)
    {
        m_pQuad->setShader(m_pBaseShader);
    }

    mainLoop();
}

void Window::setupGLVertex()
{
    // m_pTriangle = new Triangle();
    // m_pTriangle->registerBuffer();

    vec3 color = {1.f, 0.f, 0.f}; // Red color for the quad
    //  * (m_nWidth / (float)m_nHeight) only needed if not matrix translated
    m_pQuad = new Quad(0, 0, 0.5f, 0.5f, color);
}

void Window::setupShaders()
{
    m_pBaseShader = new TestShader();
}

void Window::mainLoop()
{
    m_fLastDrawTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_pWindow))
    {
        // Because mac's retina display has a different pixel ratio (and moving to different monitors)
        // need to adjust the viewport to match the actual framebuffer size.
        glfwGetFramebufferSize(m_pWindow, &m_nActualWidth, &m_nActualHeight);
        m_fRatio = m_nActualWidth / (float) m_nActualHeight;
        m_pCamera->setRatio(m_fRatio);

        glViewport(0, 0, m_nActualWidth, m_nActualHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        drawFrame();

        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }

    glfwTerminate();
}

void Window::drawFrame()
{
    m_fCurrentDrawTime = glfwGetTime();
    m_fDeltaTime = m_fCurrentDrawTime - m_fLastDrawTime;
    m_fLastDrawTime = m_fCurrentDrawTime;

    // Michael TODO: translate mouse position to camera position
    // m_pCamera->setPosition(m_fTempMouseX, m_fTempMouseY);
    // m_pCamera->move(3 * m_fDeltaTime, 0, 0); // Move the camera back a bit
    // m_pCamera->rotate(0, 0, 1.f * m_fDeltaTime); // Rotate the camera slightly each frame

    // m_pTriangle->draw();

    if (m_pQuad)
    {
        float fGlX, fGlY;
        InputManager::getMousePositionInGL(fGlX, fGlY);
        m_pQuad->setPosition(fGlX, fGlY);
        LOG_EX("Quad position set to: ({}, {})\r", fGlX, fGlY);
        m_pQuad->draw();
    }
}

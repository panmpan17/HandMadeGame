// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <linmath.h>
#include <functional>

#include "window.h"
#include "camera.h"
#include "draw/shader.h"
#include "draw/quad.h"
#include "draw/triangle.h"
#include "debug_macro.h"
#include "input_handle.h"
#include "image.h"


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
    if (m_pBaseShader)
    {
        delete m_pBaseShader;
        m_pBaseShader = nullptr;
    }
    if (m_pImageShader)
    {
        delete m_pImageShader;
        m_pImageShader = nullptr;
    }
    if (m_pDrawables)
    {
        for (int i = 0; i < m_nDrawableCount; ++i)
        {
            if (m_pDrawables[i])
            {
                delete m_pDrawables[i];
            }
        }
        delete[] m_pDrawables;
        m_pDrawables = nullptr;
    }
    if (m_pImage)
    {
        delete m_pImage;
        m_pImage = nullptr;
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
    
    m_pImage = new Image("assets/images/test.png");
    if (m_pImage->isCPULoaded())
    {
        m_pImage->loadTextureToGL();
        m_pImage->freeCPUData();
    }

    setupShaders();
    setupGLVertex();

    mainLoop();
}

void Window::setupGLVertex()
{
    m_nDrawableCount = 3;
    m_pDrawables = new IDrawable*[m_nDrawableCount];

    m_pDrawables[0] = new Triangle();
    m_pDrawables[0]->setShader(m_pBaseShader);
    m_pDrawables[0]->registerBuffer();

    vec4 red = {1.f, 0.f, 0.f, 1.f}; // Red color for the quad
    //  * (m_nWidth / (float)m_nHeight) only needed if not matrix translated
    m_pDrawables[1] = new Quad(0, 0, 0.5f, 0.5f, red);
    m_pDrawables[1]->setShader(m_pImageShader);
    static_cast<Quad*>(m_pDrawables[1])->setImage(m_pImage);
    m_pDrawables[1]->registerBuffer();

    vec4 color = {0.5f, 0.5f, 1.f, 1.f}; // Blue color for the second quad
    m_pDrawables[2] = new Quad(0.5f, 0.5f, 0.3f, 0.3f, color);
    m_pDrawables[2]->setShader(m_pImageShader);
    static_cast<Quad*>(m_pDrawables[2])->setImage(m_pImage);
    m_pDrawables[2]->registerBuffer();
}

void Window::setupShaders()
{
    m_pBaseShader = new TestShader();
    m_pImageShader = new ImageShader();
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

    for (int i = 0; i < m_nDrawableCount; ++i)
    {
        m_pDrawables[i]->draw();
    }
}

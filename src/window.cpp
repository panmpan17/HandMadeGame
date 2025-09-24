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
#include "input_handle.h"
#include "draw/image.h"
#include "draw/shader_loader.h"
#include "world.h"
#include "post_process/render_process_queue.h"
#include "models/simple_obj_reader.h"


Window* Window::ins = nullptr;

Window::Window()
{
    ins = this;

    if (!glfwInit())
    {
        LOGLN("Failed to initialize GLFW");
        return;
    }
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
    if (m_pWorldScene)
    {
        delete m_pWorldScene;
        m_pWorldScene = nullptr;
    }

    InputManager::Cleanup();
    ImageLoader::Cleanup();

    glfwTerminate();
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warnings
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::println("Debug message ({}) at {}:{}: {}", id, __FILE__, __LINE__, message);
}

void Window::configureAndCreateWindow()
{
    // Configure GL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, m_bResizable ? GLFW_TRUE : GLFW_FALSE);

    glfwSwapInterval(1);

#if IS_DEBUG_VERSION
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    // GLFWmonitor* pPrimaryMonitor = glfwGetPrimaryMonitor();
    // const GLFWvidmode* pVideoMode = glfwGetVideoMode(pPrimaryMonitor);

    // m_nWidth = pVideoMode->width;
    // m_nHeight = pVideoMode->height;

    // // Set window hints to create a borderless, resizable window
    // glfwWindowHint(GLFW_RED_BITS, pVideoMode->redBits);
    // glfwWindowHint(GLFW_GREEN_BITS, pVideoMode->greenBits);
    // glfwWindowHint(GLFW_BLUE_BITS, pVideoMode->blueBits);
    // glfwWindowHint(GLFW_REFRESH_RATE, pVideoMode->refreshRate);
    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // This is key for borderless


    m_pWindow = glfwCreateWindow(m_nWidth, m_nHeight, "My GLFW Window", NULL, NULL);
    if (!m_pWindow)
    {
        LOGLN("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwSetWindowAspectRatio(m_pWindow, m_nWidth, m_nHeight);

    glfwGetFramebufferSize(m_pWindow, &m_nActualWidth, &m_nActualHeight);
    // m_fRatio = m_nActualWidth / (float)m_nActualHeight;

    // glfwSetWindowMonitor(m_pWindow, pPrimaryMonitor, 0, 0, pVideoMode->width, pVideoMode->height, pVideoMode->refreshRate);

    // GLint flags;
    // glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    // if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    // {
    //     glEnable(GL_DEBUG_OUTPUT);
    //     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //     glDebugMessageCallback(glDebugOutput, nullptr);
    //     glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    // }
}

void Window::start()
{
    InputManager::Initialize();
    ImageLoader::Initialize();
    m_pCamera = new Camera();

    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_ESCAPE, [this](bool pressed) {
        if (pressed)
        {
            m_bRenderProcessQueueUseSplit = !m_bRenderProcessQueueUseSplit;
        }
    });

    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_R, [](bool pressed) {
        if (pressed)
        {
            LOGLN("Reloading all shaders...");
            ShaderLoader::getInstance()->reloadAllShaders();
        }
    });

    glfwSetKeyCallback(m_pWindow, &InputManager::onKeyCallback);
    glfwSetCursorEnterCallback(m_pWindow, &InputManager::onMouseEnterCallback);
    glfwSetCursorPosCallback(m_pWindow, &InputManager::onMousePosCallback);
    glfwSetMouseButtonCallback(m_pWindow, &InputManager::onMouseButtonCallback);

    // glfwSetWindowOpacity(m_pWindow, 0.5f); // Fun

    glfwMakeContextCurrent(m_pWindow);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1); // Enable vsync

    ShaderLoader::Initialize();
    
    ImageLoader::getInstance()->registerImage("test", "assets/images/test.png");
    ImageLoader::getInstance()->registerImage("dust", "assets/images/dust_1.png");
    ImageLoader::getInstance()->registerImage("character", "assets/images/character_animation.png");
    ImageLoader::getInstance()->registerImage("cover_test", "assets/images/cover_test_3.jpg");

    m_pRenderProcessQueue = new RenderProcessQueue(this);
    m_pRenderProcessQueue->setupProcesses();

    m_pWorldScene = new WorldScene();
    m_pWorldScene->bloomTest();
    // m_pWorldScene->createPinPongGame();
    // m_pWorldScene->init();
    // m_pWorldScene->clearAllNodes();
    // m_pWorldScene->readFromFiles("assets/level.txt");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mainLoop();
}

void Window::mainLoop()
{
    m_fLastDrawTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_pWindow))
    {
        // Because mac's retina display has a different pixel ratio (and moving to different monitors)
        // need to adjust the viewport to match the actual framebuffer size.
        glfwGetFramebufferSize(m_pWindow, &m_nActualWidth, &m_nActualHeight);
        float fNewRatio = m_nActualWidth / (float) m_nActualHeight;
        if (m_fRatio != fNewRatio)
        {
            m_fRatio = fNewRatio;
            m_pCamera->setRatio(m_fRatio);
        }

        m_fCurrentDrawTime = glfwGetTime();
        m_fDeltaTime = m_fCurrentDrawTime - m_fLastDrawTime;
        m_fLastDrawTime = m_fCurrentDrawTime;

        m_pWorldScene->update(m_fDeltaTime);

        drawFrame();

        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }

    glfwTerminate();
}

void Window::drawFrame()
{
    m_nDrawCallCount = 0;

    if (true) // Enable post process
    {
        m_pRenderProcessQueue->beginFrame();
        m_pWorldScene->render();
        m_pRenderProcessQueue->endFrame();
        m_pRenderProcessQueue->startProcessing();

        if (m_bRenderProcessQueueUseSplit)
        {
            m_pRenderProcessQueue->renderToScreenSplit();
        }
        else
        {
            m_pRenderProcessQueue->renderToScreen();
        }
    }
    else
    {
        glViewport(0, 0, m_nActualWidth, m_nActualHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        m_pWorldScene->render();
    }
    LOG_EX("Draw call count: {}, Fps: {}\r", m_nDrawCallCount, 1.0f / m_fDeltaTime);
}

void Window::increaseDrawCallCount()
{
    ++m_nDrawCallCount;
}

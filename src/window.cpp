// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <linmath.h>
#include <functional>

#include "window.h"
#include "input_handle.h"
#include "light_manager.h"
#include "draw/image.h"
#include "draw/shader_loader.h"
#include "world.h"
#include "post_process/render_process_queue.h"
#include "models/simple_obj_reader.h"
#include "editor/camera_inspector.h"
#include "editor/hierarchy_view.h"
#include "editor/post_process_inspector.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "camera.h"


Window* Window::ins = nullptr;

Window::Window()
{
    ins = this;

    m_bShowIMGUI = true;

    if (!glfwInit())
    {
        LOGLN("Failed to initialize GLFW");
        return;
    }
}

Window::~Window()
{
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
    LightManager::Cleanup();

    glfwTerminate();

    if (m_bShowIMGUI)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warnings
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    LOGLN_EX("Debug message ({}) at {}:{}: {}", id, __FILE__, __LINE__, message);
}

void Window::configureAndCreateWindow()
{
    // Configure GL version
#if __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, m_bResizable ? GLFW_TRUE : GLFW_FALSE);

    glfwWindowHint(GLFW_DEPTH_BITS, 24);

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


    m_pWindow = glfwCreateWindow(m_nWidth, m_nHeight, "Michael Hand Made Game", NULL, NULL);
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

#if !__APPLE__
    // GLint flags;
    // glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    // if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    // {
    //     glEnable(GL_DEBUG_OUTPUT);
    //     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //     glDebugMessageCallback(glDebugOutput, nullptr);
    //     glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    // }
#endif
}

void Window::start()
{
    InputManager::Initialize();
    ImageLoader::Initialize();

    // InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_R, [](bool pressed) {
    //     if (pressed)
    //     {
    //         LOGLN("Reloading all shaders...");
    //         ShaderLoader::getInstance()->reloadAllShaders();
    //     }
    // });

    glfwSetKeyCallback(m_pWindow, &InputManager::onKeyCallback);
    glfwSetCursorEnterCallback(m_pWindow, &InputManager::onMouseEnterCallback);
    glfwSetCursorPosCallback(m_pWindow, &InputManager::onMousePosCallback);
    glfwSetMouseButtonCallback(m_pWindow, &InputManager::onMouseButtonCallback);

    // glfwSetWindowOpacity(m_pWindow, 0.5f); // Fun

    glfwMakeContextCurrent(m_pWindow);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1); // Enable vsync

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);


    // Setup Dear ImGui context
    if (m_bShowIMGUI)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init();
    }

    LightManager::Initialize();

    ShaderLoader::Initialize();
    
    ImageLoader::getInstance()->registerImage("test", "assets/images/test.png");
    ImageLoader::getInstance()->registerImage("dust", "assets/images/dust_1.png");
    ImageLoader::getInstance()->registerImage("character", "assets/images/character_animation.png");
    ImageLoader::getInstance()->registerImage("cover_test", "assets/images/cover_test_3.jpg");
    ImageLoader::getInstance()->registerImage("box_uv", "assets/images/box_test_uv.jpg");

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

    m_oEditorWindows.addElement(new CameraInspector());
    m_oEditorWindows.addElement(new HierarchyView());
    m_oEditorWindows.addElement(new PostProcessInspector());

    beforeLoop();
    mainLoop();
}

void Window::beforeLoop()
{
    m_fRatio = m_nActualWidth / (float) m_nActualHeight;

    m_pWorldScene->onStart();
}

void Window::mainLoop()
{
    m_fLastDrawTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_pWindow))
    {
        glfwPollEvents();

        // Because mac's retina display has a different pixel ratio (and moving to different monitors)
        // need to adjust the viewport to match the actual framebuffer size.
        glfwGetFramebufferSize(m_pWindow, &m_nActualWidth, &m_nActualHeight);
        float fNewRatio = m_nActualWidth / (float) m_nActualHeight;
        if (m_fRatio != fNewRatio)
        {
            m_fRatio = fNewRatio;
            // m_pCamera->setRatio(m_fRatio);
            // TODO: send event to update all cameras
        }


        if (m_bShowIMGUI)
        {
            mainLoop_IMGUI();
        }

        m_fCurrentDrawTime = glfwGetTime();
        m_fDeltaTime = m_fCurrentDrawTime - m_fLastDrawTime;
        m_fLastDrawTime = m_fCurrentDrawTime;

        m_pWorldScene->update(m_fDeltaTime);

        drawFrame();

        if (m_bShowIMGUI)
        {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(m_pWindow);
    }
}

void Window::mainLoop_IMGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    int nSize = m_oEditorWindows.getSize();
    for (int i = 0; i < nSize; ++i)
    {
        IEditorWindow* pWindow = m_oEditorWindows.getElement(i);
        if (pWindow && pWindow->isActive())
        {
            pWindow->update(m_fDeltaTime);
        }
    }

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Editor Windows"))
        {
            for (int i = 0; i < nSize; ++i)
            {
                IEditorWindow* pWindow = m_oEditorWindows.getElement(i);
                if (pWindow)
                {
                    if (ImGui::MenuItem(typeid(*pWindow).name(), NULL, pWindow->isActive()))
                    {
                        pWindow->setActive(!pWindow->isActive());
                    }
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Window::drawFrame()
{
    m_nDrawCallCount = 0;

    if (Camera::main)
    {
        Camera::main->updateCameraDataBuffer();
    }

    if (m_bEnablePostProcess) // Enable post process
    {
        m_pRenderProcessQueue->beginFrame();
        m_pWorldScene->render();
        m_pRenderProcessQueue->endFrame();

        glDisable(GL_DEPTH_TEST);
        m_pRenderProcessQueue->startProcessing();
        m_pRenderProcessQueue->renderToScreen();
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_nActualWidth, m_nActualHeight);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_pWorldScene->render();
    }

#if IS_DEBUG_VERSION
    if (m_bShowIMGUI)
    {
        drawFrameInfo();
    }
#endif
}

void Window::drawFrameInfo()
{
    ImGui::Begin("Info", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar
                 | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
                 | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground);
    ImGui::SetWindowSize(ImVec2(200, 100), ImGuiCond_Always);
    ImGui::SetWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 45), ImGuiCond_Always);
    ImGui::Text("%.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Text("Draw Call Count: %d", m_nDrawCallCount);
    ImGui::End();
}

void Window::increaseDrawCallCount()
{
    ++m_nDrawCallCount;
}

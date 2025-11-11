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
#include "engine_event_dispatcher.h"
#include "input/input_manager.h"
#include "scene/world.h"
#include "../render/image.h"
#include "../render/shader_loader.h"
#include "../render/vertex.h"
#include "../render/post_process/render_process_queue.h"
#include "../render/lighting/light_manager.h"
#include "../render/lighting/direction_light.h"
#include "../misc/preference.h"
#include "../../editor/node_inspector.h"
#include "../../editor/hierarchy_view.h"
#include "../../editor/post_process_inspector.h"
#include "../../utils/file_watch_dog.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


Window* Window::ins = nullptr;

Window::Window()
{
    ins = this;

    m_bShowIMGUI = Preference::getEnableIMGUI();
    m_bEnablePostProcess = Preference::getEnablePostProcess();
    m_oWindowSize.x = Preference::getWindowWidth();
    m_oWindowSize.y = Preference::getWindowHeight();

    if (!glfwInit())
    {
        LOGLN("Failed to initialize GLFW");
        return;
    }
}

Window::~Window()
{
    if (m_pFileWatchDog)
    {
        m_pFileWatchDog->stopWatching();
        delete m_pFileWatchDog;
        m_pFileWatchDog = nullptr;
    }

    if (m_pWindow)
    {
        int nWindowX, nWindowY;
        glfwGetWindowPos(m_pWindow, &nWindowX, &nWindowY);
        Preference::setWindowPositionX(nWindowX);
        Preference::setWindowPositionY(nWindowY);

        glfwGetWindowSize(m_pWindow, &m_oWindowSize.x, &m_oWindowSize.y);
        Preference::setWindowWidth(m_oWindowSize.x);
        Preference::setWindowHeight(m_oWindowSize.y);

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

    Preference::savePreferences();

    glfwTerminate();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warnings
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    LOGLN("Debug message ({}) at {}:{}: {}", id, __FILE__, __LINE__, message);
}

bool Window::configureAndCreateWindow()
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


    m_pWindow = glfwCreateWindow(m_oWindowSize.x, m_oWindowSize.y, "Michael Hand Made Game", NULL, NULL);
    if (!m_pWindow)
    {
        LOGLN("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    glfwSetWindowPos(m_pWindow, Preference::getWindowPositionX(), Preference::getWindowPositionY());

    if (m_bKeepRatio)
    {
        glfwSetWindowAspectRatio(m_pWindow, m_oWindowSize.x, m_oWindowSize.y);
    }

    glfwGetFramebufferSize(m_pWindow, &m_oActualSize.x, &m_oActualSize.y);
    // m_fRatio = m_oActualSize.x / (float)m_oActualSize.y;

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

    // glfwSetWindowOpacity(m_pWindow, 0.5f); // Fun
    glfwMakeContextCurrent(m_pWindow);
    glfwSwapInterval(1); // Enable vsync


    // Set up GLAD
    gladLoadGL(glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void Window::setupManagers()
{
    ImageLoader::Initialize();
    LightManager::Initialize();
    ShaderLoader::Initialize();    

    setupInputManager();
    setupIMGUIAndEditorWindows();

    m_pRenderProcessQueue = new RenderProcessQueue(this);

    m_pWorldScene = new WorldScene();
    m_pWorldScene->init();

#if IS_DEBUG_VERSION
    m_pFileWatchDog = new FileWatchDog("assets/");
    m_pFileWatchDog->setFileChangeCallback([](const std::string& strFilePath, eFileChangeType eType) {
        EngineEventDispatcher::getInstance().runOnMainThread([strFilePath, eType]() {
            ShaderLoader::getInstance()->onFileChangedListener(strFilePath, eType);
        });
    });
    m_pFileWatchDog->startWatching();
#endif
}

void Window::setupInputManager()
{
    InputManager::Initialize();

    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_FUNCTION_3, [](bool pressed) {
        if (pressed)
        {
            Window::ins->m_bShowIMGUI = !Window::ins->m_bShowIMGUI;
            Preference::setEnableIMGUI(Window::ins->m_bShowIMGUI);
        }
    });

    glfwSetKeyCallback(m_pWindow, &InputManager::onKeyCallback);
    glfwSetCursorEnterCallback(m_pWindow, &InputManager::onMouseEnterCallback);
    glfwSetCursorPosCallback(m_pWindow, &InputManager::onMousePosCallback);
    glfwSetMouseButtonCallback(m_pWindow, &InputManager::onMouseButtonCallback);
}

void Window::setupIMGUIAndEditorWindows()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    m_oEditorWindows.addElement(new NodeInspector());
    m_oEditorWindows.addElement(new HierarchyView());
    m_oEditorWindows.addElement(new PostProcessInspector());
}

void Window::beforeLoop()
{
    m_fRatio = m_oActualSize.x / (float) m_oActualSize.y;

    m_pWorldScene->onStart();
}

void Window::mainLoop()
{
    beforeLoop();

    m_fLastDrawTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_pWindow))
    {
        glfwPollEvents();

        // Because mac's retina display has a different pixel ratio (and moving to different monitors)
        // need to adjust the viewport to match the actual framebuffer size.
        glfwGetFramebufferSize(m_pWindow, &m_oActualSize.x, &m_oActualSize.y);
        float fNewRatio = m_oActualSize.x / (float) m_oActualSize.y;
        if (m_fRatio != fNewRatio)
        {
            m_fRatio = fNewRatio;
            m_onWindowResize.invoke(Vector2i(m_oActualSize.x, m_oActualSize.y));

            glfwGetWindowSize(m_pWindow, &m_oWindowSize.x, &m_oWindowSize.y);
        }

        runUpdate();
        drawFrame();

        glfwSwapBuffers(m_pWindow);
    }
}

void Window::runUpdate()
{
    if (m_bShowIMGUI)
    {
        updateIMGUI();
    }

    m_fCurrentDrawTime = glfwGetTime();
    m_fDeltaTime = m_fCurrentDrawTime - m_fLastDrawTime;
    m_fLastDrawTime = m_fCurrentDrawTime;

    EngineEventDispatcher::getInstance().updateEvents(m_fDeltaTime);

    m_pWorldScene->update(m_fDeltaTime);
}

void Window::updateIMGUI()
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

    LightManager* const pLightManager = LightManager::getInstance();
    pLightManager->updateLightingUBO();

    DirectionLightComponent* pMainDirLight = pLightManager->getMainDirectionLightComponent();
    if (pMainDirLight && pMainDirLight->getShadowsEnabled())
    {
        glViewport(0, 0, LightManager::SHADOW_MAP_WIDTH, LightManager::SHADOW_MAP_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, pLightManager->getShadowDepthMapFBO());
        glClear(GL_DEPTH_BUFFER_BIT);
        m_pWorldScene->renderDepth();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        glViewport(0, 0, m_oActualSize.x, m_oActualSize.y);
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_pWorldScene->render();
    }

    if (m_bShowIMGUI)
    {
#if IS_DEBUG_VERSION
        drawFrameInfo();
#endif
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
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

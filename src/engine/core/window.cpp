// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <objc/runtime.h>
#include <objc/message.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <linmath.h>
#include <functional>

#include "window.h"
#include "camera.h"
#include "time.h"
#include "engine_event_dispatcher.h"
#include "input/input_manager.h"
#include "scene/world.h"
// #include "../render/image_loader.h"
#include "../render/shader_loader.h"
// #include "../render/material_loader.h"
#include "../render/vertex.h"
// #include "../render/post_process/render_process_queue.h"
// #include "../render/lighting/light_manager.h"
// #include "../render/lighting/direction_light.h"
#include "../render/font/font_loader.h"
#include "../misc/preference.h"
// #include "../../editor/gizmos.h"
// #include "../../editor/node_inspector.h"
// #include "../../editor/hierarchy_view.h"
// #include "../../editor/post_process_inspector.h"
#include "../../utils/file_watch_dog.h"

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

#if __APPLE__
#include "metal/helper.h"
#endif // __APPLE__


inline constexpr std::string_view PROFILER_TAG_WINDOW_INITIALIZATION = "WindowInitialization";
#define DEBUG_WINDOW_INIT_TIMER(strMsg) PROFILER_END_TIMER(PROFILER_TAG_WINDOW_INITIALIZATION, strMsg)


// This is a workaround for Metal PixelFormat enum not being properly recognized in VSCode intellisense.
#if VSCODE_ONLY
namespace MTL { enum PixelFormat : NS::UInteger { PixelFormatBGRA8Unorm = 80 }; }
#endif // VSCODE_ONLY


Window* Window::ins = nullptr;

Window::Window()
{
    ins = this;

    m_bShowIMGUI = Preference::getEnableIMGUI();
    m_bEnablePostProcess = Preference::getEnablePostProcess();
    m_oWindowSize.x = Preference::getWindowWidth();
    m_oWindowSize.y = Preference::getWindowHeight();
    m_bDrawGizmos = Preference::getEnableGizmos();

#if __APPLE__
    m_pMetalDevice = MTL::CreateSystemDefaultDevice();
#endif

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

#if __APPLE__
    if (m_pMetalLayer)
    {
        m_pMetalLayer->release();
        m_pMetalLayer = nullptr;
    }
    if (m_pMetalDevice)
    {
        m_pMetalDevice->release();
        m_pMetalDevice = nullptr;
    }
#endif // __APPLE__

    if (m_pWorldScene)
    {
        delete m_pWorldScene;
        m_pWorldScene = nullptr;
    }

    InputManager::Cleanup();
    // ImageLoader::Cleanup();
    // LightManager::Cleanup();

    Preference::savePreferences();

    glfwTerminate();

    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warnings
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    LOGLN("Debug message ({}) at {}:{}: {}", id, __FILE__, __LINE__, message);
}

bool Window::configureAndCreateWindow()
{
    PROFILER_START_TIMER();

    glfwWindowHint(GLFW_RESIZABLE, m_bResizable ? GLFW_TRUE : GLFW_FALSE);

    glfwWindowHint(GLFW_DEPTH_BITS, 24);

#if __APPLE__
    if (m_pMetalDevice)
    {
        configureGLFWWithMetal();
    }
    else
    {
        configureGLFWWithOpenGL();
    }
#else
    configureGLFWWithOpenGL();
#endif

#if IS_DEBUG_VERSION
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // IS_DEBUG_VERSION

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

    // glfwSetWindowOpacity(m_pWindow, 0.5f); // Fun

    DEBUG_WINDOW_INIT_TIMER("Window configured");

    return true;
}

void Window::configureGLFWWithOpenGL()
{
    LOGLN("Configuring GLFW with OpenGL");
#if __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif // __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::configureGLFWWithMetal()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void Window::initializeGraphicsAPI()
{
#if __APPLE__
    if (m_pMetalDevice)
    {
        LOGLN("Metal Device found: {}", m_pMetalDevice->name()->utf8String());
        bindMetalToGlfwWindow();
    }
    else
    {
        LOGLN("Metal is not supported on this device.");
        bindOpenGLToGlfwWindow();
    }

#else
    bindOpenGLToGlfwWindow();
#endif // __APPLE__
}

void Window::bindOpenGLToGlfwWindow()
{
    glfwMakeContextCurrent(m_pWindow);
    glfwSwapInterval(1); // Enable vsync

    gladLoadGL(glfwGetProcAddress);

    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::bindMetalToGlfwWindow()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    void* pNSWindow = glfwGetCocoaWindow(m_pWindow);

    // 2. Get the "contentView" of the window
    // Equivalent to: NSView* view = [nsWindow contentView];
    void* view = ((void* (*)(id, SEL))objc_msgSend)((id)pNSWindow, sel_registerName("contentView"));

    // 3. Create the Metal Layer using metal-cpp
    m_pMetalLayer = CA::MetalLayer::layer();
    m_pMetalLayer->setDevice(m_pMetalDevice);

    // the PixelFormat is defined in MTLPixelFormat.hpp, for some reason the enum type is not compiled correctly
    m_pMetalLayer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);

    // Use the window's scale factor for Retina displays
    m_pMetalLayer->setDrawableSize(CGSizeMake(m_oActualSize.x, m_oActualSize.y)); // TODO: Should this be m_oActualSize or m_oWindowSize?

    // 4. Attach the Metal Layer to the View (The "Bridge")
    // Equivalent to: [view setLayer:outLayer];
    ((void (*)(id, SEL, id))objc_msgSend)((id)view, sel_registerName("setLayer:"), (id)m_pMetalLayer);

    // 5. Tell the view to host the layer
    // Equivalent to: [view setWantsLayer:YES];
    ((void (*)(id, SEL, BOOL))objc_msgSend)((id)view, sel_registerName("setWantsLayer:"), (BOOL)true);

    // // TODO: setOpaque true?
    // ((void (*)(id, SEL, BOOL))objc_msgSend)((id)view, sel_registerName("setOpaque:"), (BOOL)true);
    
    // 6. Set layer resizing policy (so it resizes with window)
    // kCALayerWidthSizable | kCALayerHeightSizable = 2 | 16 = 18
    // m_pMetalLayer->setAutoresizingMask(18);

    m_pMetalCommandQueue = m_pMetalDevice->newCommandQueue();
}

void Window::setWindowSize(int nWidth, int nHeight)
{
    m_oWindowSize.x = nWidth;
    m_oWindowSize.y = nHeight;
    if (m_pWindow)
    {
        glfwSetWindowSize(m_pWindow, nWidth, nHeight);
    }
}

void Window::setupManagers()
{
    PROFILER_START_TIMER();

    TimeManager::Initialize();
    PROFILER_END_TIMER("Initialization", "TimeManager setup");
    // ImageLoader::Initialize();
    // PROFILER_END_TIMER("Initialization", "Image setup");
    // LightManager::Initialize();
    // PROFILER_END_TIMER("Initialization", "Lighting setup");
    ShaderLoader::Initialize();
    PROFILER_END_TIMER("Initialization", "Shader setup");
    // MaterialLoader::Initialize();
    // PROFILER_END_TIMER("Initialization", "Material setup");

    setupInputManager();
    PROFILER_END_TIMER("Initialization", "Input manager");

    // GizmosManager::Initialize();
    // PROFILER_END_TIMER("Initialization", "Gizmos setup");

    // FontLoader::Initialize();
    // FontLoader::getInstance()->loadFont("assets/fonts/arial.ttf");

    // m_pRenderProcessQueue = new RenderProcessQueue(this);

    PROFILER_END_TIMER("Initialization", "Render process queue setup");

    m_pWorldScene = new WorldScene();

    PROFILER_END_TIMER("Initialization", "world setup");
}

void Window::setupInputManager()
{
    InputManager::Initialize();
    glfwSetKeyCallback(m_pWindow, &InputManager::onKeyCallback);
    glfwSetCursorEnterCallback(m_pWindow, &InputManager::onMouseEnterCallback);
    glfwSetCursorPosCallback(m_pWindow, &InputManager::onMousePosCallback);
    glfwSetMouseButtonCallback(m_pWindow, &InputManager::onMouseButtonCallback);
}

void Window::setupGameEngineRelatedObject()
{
    PROFILER_START_TIMER();

    m_pWorldScene->init();

    PROFILER_END_TIMER("Initialization", "World init");

    setupIMGUIAndEditorWindows();

    PROFILER_END_TIMER("Initialization", "IMGui & editor setup");

    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_FUNCTION_3, [](bool pressed) {
        if (pressed)
        {
            Window::ins->m_bShowIMGUI = !Window::ins->m_bShowIMGUI;
            Preference::setEnableIMGUI(Window::ins->m_bShowIMGUI);
        }
    });

    PROFILER_END_TIMER("Initialization", "Input key setup");

#if IS_DEBUG_VERSION
    m_pFileWatchDog = new FileWatchDog("assets/");
    m_pFileWatchDog->setFileChangeCallback([](const std::string& strFilePath, eFileChangeType eType) {
        EngineEventDispatcher::getInstance().runOnMainThread([strFilePath, eType]() {
            // ShaderLoader::getInstance()->onFileChangedListener(strFilePath, eType);
            // MaterialLoader::getInstance()->onFileChangedListener(strFilePath, eType);
        });
    });
    m_pFileWatchDog->startWatching();

    PROFILER_END_TIMER("Initialization", "IMGui setup");
#endif
}

void Window::setupIMGUIAndEditorWindows()
{
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // // Setup Platform/Renderer backends
    // ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    // ImGui_ImplOpenGL3_Init();

    // if (m_bAddGameRelatedIMGUIWindows)
    // {
    //     // m_oEditorWindows.addElement(new NodeInspector());
    //     m_oEditorWindows.addElement(new HierarchyView());
    //     // m_oEditorWindows.addElement(new PostProcessInspector());
    // }

    // for (int i = 0; i < m_oEditorWindows.getSize(); ++i)
    // {
    //     IEditorWindow* pWindow = m_oEditorWindows.getElement(i);
    //     if (pWindow)
    //     {
    //         bool bActive = Preference::getPlayerPreferenceInstance().getBool(std::string("EditorWindow_") + typeid(*pWindow).name(), true);
    //         pWindow->setActive(bActive);
    //     }
    // }
}

void Window::beforeLoop()
{
    m_fRatio = m_oActualSize.x / (float) m_oActualSize.y;

    TimeManager::getInstance()->onWindowStart();
}

void Window::mainLoop()
{
    beforeLoop();

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
            m_onWindowResize.invoke(m_oActualSize);

            glfwGetWindowSize(m_pWindow, &m_oWindowSize.x, &m_oWindowSize.y);

            m_pMetalLayer->setDrawableSize(CGSizeMake(m_oActualSize.x, m_oActualSize.y));
        }

        runUpdate();

        if (m_pMetalDevice)
        {
            NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

            CA::MetalDrawable* pDrawable = m_pMetalLayer->nextDrawable();

            if (pDrawable)
            {
                MTL::CommandBuffer* pCommandBuffer = m_pMetalCommandQueue->commandBuffer();

                MTL::RenderPassDescriptor* pRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
                MTL::RenderPassColorAttachmentDescriptor* pColorAttachment = pRenderPassDescriptor->colorAttachments()->object(0);

                pColorAttachment->setTexture(pDrawable->texture());
                pColorAttachment->setLoadAction(MTL::LoadActionClear);
                pColorAttachment->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));
                pColorAttachment->setStoreAction(MTL::StoreActionStore);

                m_pCurrentFrameRenderEncoder = pCommandBuffer->renderCommandEncoder(pRenderPassDescriptor);

                drawFrame();

                m_pCurrentFrameRenderEncoder->endEncoding();

                pCommandBuffer->presentDrawable(pDrawable);
                pCommandBuffer->commit();

                pRenderPassDescriptor->release();
            }

            pPool->release();
        }
        else
        {
            drawFrame();
            glfwSwapBuffers(m_pWindow);
        }
    }
}

void Window::runUpdate()
{
    if (m_bShowIMGUI)
    {
        updateIMGUI();
    }


    float fDeltaTime = TimeManager::getInstance()->preUpdate();

    EngineEventDispatcher::getInstance().updateEvents(fDeltaTime);

    m_pWorldScene->update(fDeltaTime);
}

void Window::updateIMGUI()
{
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
    
    // int nSize = m_oEditorWindows.getSize();
    // for (int i = 0; i < nSize; ++i)
    // {
    //     IEditorWindow* pWindow = m_oEditorWindows.getElement(i);
    //     if (pWindow && pWindow->isActive())
    //     {
    //         pWindow->update();
    //     }
    // }

    // if (ImGui::BeginMainMenuBar())
    // {
    //     if (ImGui::BeginMenu("Editor Windows"))
    //     {
    //         for (int i = 0; i < nSize; ++i)
    //         {
    //             IEditorWindow* pWindow = m_oEditorWindows.getElement(i);
    //             if (pWindow)
    //             {
    //                 if (ImGui::MenuItem(typeid(*pWindow).name(), NULL, pWindow->isActive()))
    //                 {
    //                     pWindow->setActive(!pWindow->isActive());
    //                     Preference::getPlayerPreferenceInstance().setBool(std::string("EditorWindow_") + typeid(*pWindow).name(), pWindow->isActive());
    //                 }
    //             }
    //         }

    //         ImGui::EndMenu();
    //     }

    //     if (ImGui::MenuItem("Show Gizmos", NULL, m_bDrawGizmos))
    //     {
    //         m_bDrawGizmos = !m_bDrawGizmos;
    //         Preference::setEnableGizmos(m_bDrawGizmos);
    //     }

    //     ImGui::EndMainMenuBar();
    // }
}

void Window::drawFrame()
{
    m_nDrawCallCount = 0;
    m_nTriangleCount = 0;

    if (Camera::main)
    {
        // Camera::main->updateCameraDataBuffer();
    }

    // LightManager* const pLightManager = LightManager::getInstance();
    // pLightManager->updateLightingUBO();

    // DirectionLightComponent* pMainDirLight = pLightManager->getMainDirectionLightComponent();
    // if (pMainDirLight && pMainDirLight->getShadowsEnabled())
    // {
    //     glViewport(0, 0, LightManager::SHADOW_MAP_WIDTH, LightManager::SHADOW_MAP_HEIGHT);
    //     glBindFramebuffer(GL_FRAMEBUFFER, pLightManager->getShadowDepthMapFBO());
    //     glClear(GL_DEPTH_BUFFER_BIT);
    //     m_pWorldScene->renderDepth();
    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // }

    // if (m_bEnablePostProcess) // Enable post process
    // {
    //     // m_pRenderProcessQueue->beginFrame();
    //     m_pWorldScene->render();
    //     m_pRenderProcessQueue->endFrame();

    //     glDisable(GL_DEPTH_TEST);
    //     m_pRenderProcessQueue->startProcessing();
    //     m_pRenderProcessQueue->renderToScreen();
    //     glEnable(GL_DEPTH_TEST);
    // }
    // else
    {
        // glViewport(0, 0, m_oActualSize.x, m_oActualSize.y);
        // glClearColor(0.f, 0.f, 0.f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_pWorldScene->render();
    }

    if (m_bDrawGizmos)
    {
        m_pWorldScene->drawGizmos();
    }

    if (m_bShowIMGUI)
    {
#if IS_DEBUG_VERSION
        if (m_bShowFPS)
        {
            drawFrameInfo();
        }
#endif
        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void Window::drawFrameInfo()
{
    // ImGui::Begin("Info", nullptr,
    //              ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar
    //              | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
    //              | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground);
    // ImGui::SetWindowSize(ImVec2(200, 100), ImGuiCond_Always);
    // ImGui::SetWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 45), ImGuiCond_Always);
    // ImGui::Text("%.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    // ImGui::Text("Draw Call: %d; Triangle: %d", m_nDrawCallCount, m_nTriangleCount);
    // ImGui::End();
}

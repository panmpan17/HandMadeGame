// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <objc/runtime.h>
#include <objc/message.h>
#endif // __APPLE__

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <linmath.h>
#include <functional>

#include "window.h"
#include "camera.h"
#include "time.h"
#include "imgui_editor_addon.h"
#include "engine_event_dispatcher.h"
#include "input/input_manager.h"
#include "scene/world.h"
#include "../render/core/renderer.h"
#include "../render/image_loader.h"
#include "../render/shader_loader.h"
#include "../render/material_loader.h"
#include "../render/post_process/render_process_queue.h"
#include "../render/lighting/light_manager.h"
#include "../render/lighting/direction_light.h"
#include "../render/font/font_loader.h"
#include "../misc/preference.h"
#include "../../editor/gizmos.h"
#include "../../utils/file_watch_dog.h"



inline constexpr std::string_view PROFILER_TAG_WINDOW_INITIALIZATION = "WindowInitialization";
#define DEBUG_WINDOW_INIT_TIMER(strMsg) PROFILER_END_TIMER(PROFILER_TAG_WINDOW_INITIALIZATION, strMsg)


// This is a workaround for Metal PixelFormat enum not being properly recognized in VSCode intellisense.
#if VSCODE_ONLY && __APPLE__
namespace MTL { enum PixelFormat : NS::UInteger { PixelFormatBGRA8Unorm = 80 }; }
#define GLFW_TRANSPARENT_FRAMEBUFFER 0x0002000A
#endif // VSCODE_ONLY


Window* Window::ins = nullptr;

Window::Window()
{
    ins = this;

#if IS_DEBUG_VERSION
    sm_bRestartRequested = false;
#endif // IS_DEBUG_VERSION

    m_bEnablePostProcess = Preference::getEnablePostProcess();
    m_oWindowSize.x = Preference::getWindowWidth();
    m_oWindowSize.y = Preference::getWindowHeight();
    m_bDrawGizmos = Preference::getEnableGizmos();

#if __APPLE__
    if (!Preference::getForceOpenGLOnMac())
    {
        m_pMetalDevice = MTL::CreateSystemDefaultDevice();
    }
    if (m_pMetalDevice)
    {
        Renderer::setGraphicAPI(GraphicAPI::Metal);
    }
    else
    {
        Renderer::setGraphicAPI(GraphicAPI::OpenGL);
    }
#else
    Renderer::setGraphicAPI(GraphicAPI::OpenGL);
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
    MetalRenderer::cleanup();
#endif // __APPLE__

    if (m_pWorldScene)
    {
        delete m_pWorldScene;
        m_pWorldScene = nullptr;
    }

    InputManager::Cleanup();
    ImageLoader::Cleanup();
    LightManager::Cleanup();
    ShaderLoader::Cleanup();
    MaterialLoader::Cleanup();
    GizmosManager::Cleanup();

    Preference::savePreferences();

    glfwTerminate();

    if (m_pImGuiEditorAddon)
    {
        delete m_pImGuiEditorAddon;
        m_pImGuiEditorAddon = nullptr;
    }

    ins = nullptr;
}


#pragma region Window Configuration and Graphics API Initialization


bool Window::configureAndCreateWindow()
{
    PROFILER_START_TIMER();

    configureGLFWBeforeWindowCreation();

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
    m_fRatio = m_oActualSize.x / (float) m_oActualSize.y;

    DEBUG_WINDOW_INIT_TIMER("Window configured");

    initializeGraphicsAPI();

    DEBUG_WINDOW_INIT_TIMER("Graphics API initialized");

    return true;
}

void Window::configureGLFWBeforeWindowCreation()
{
    // General GLFW configuration
    glfwWindowHint(GLFW_RESIZABLE, m_bResizable ? GLFW_TRUE : GLFW_FALSE);

    if (m_bTransparentBackground)
    {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // This is key for borderless
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    }


    // Configure GLFW based on the selected Graphic API
    if (Renderer::isUsingOpenGL())
    {
        LOGLN("Configuring GLFW for OpenGL");
#if __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif // __APPLE__
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_DEPTH_BITS, 24);

#if IS_DEBUG_VERSION
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // IS_DEBUG_VERSION
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        LOGLN("Configuring GLFW for Metal");
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
#endif // __APPLE__
}

void Window::initializeGraphicsAPI()
{
    if (Renderer::isUsingOpenGL())
    {
        bindOpenGLToGlfwWindow();
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        bindMetalToGlfwWindow();

        m_pMetalCommandQueue = m_pMetalDevice->newCommandQueue();

        MetalRenderer::initMetalDepthTexture(m_pMetalDevice, m_oActualSize.x, m_oActualSize.y);
        MetalRenderer::initRenderPassDescriptor();
        MetalRenderer::initDepthOnlyRenderPassDescriptor();
        MetalRenderer::initColorOnlyRenderPassDescriptor();
    }
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

#if __APPLE__
void Window::bindMetalToGlfwWindow()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    void* pNSWindow = glfwGetCocoaWindow(m_pWindow);

    void* view = ((void* (*)(id, SEL))objc_msgSend)((id)pNSWindow, sel_registerName("contentView"));

    m_pMetalLayer = CA::MetalLayer::layer();
    m_pMetalLayer->setDevice(m_pMetalDevice);
    m_pMetalLayer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm); // TODO: Might need to make this HDR?
    m_pMetalLayer->setDrawableSize(CGSizeMake(m_oActualSize.x, m_oActualSize.y)); // TODO: Should this be m_oActualSize or m_oWindowSize?

    ((void (*)(id, SEL, id))objc_msgSend)((id)view, sel_registerName("setLayer:"), (id)m_pMetalLayer);
    ((void (*)(id, SEL, BOOL))objc_msgSend)((id)view, sel_registerName("setWantsLayer:"), (BOOL)true);
    ((void (*)(id, SEL, BOOL))objc_msgSend)((id)view, sel_registerName("setOpaque:"), (BOOL)!getTransparentBackground());
    // TODO: set transparent color for metal
    // ((void (*)(id, SEL, BOOL))objc_msgSend)((id)view, sel_registerName("setBackgroundColor:"), (BOOL)(getTransparentBackground() ? [NSColor clearColor] : [NSColor whiteColor]));
}
#endif

#pragma endregion Window Configuration and Graphics API Initialization


#pragma region Managers and Game Engine Related Setup

void Window::setupManagers()
{
    PROFILER_START_TIMER();

    TimeManager::Initialize();

#if __APPLE__
    MetalRenderer::initializeSamplers(m_pMetalDevice);
#endif // __APPLE__

    PROFILER_END_TIMER("Initialization", "TimeManager setup");
    ImageLoader::Initialize();
    PROFILER_END_TIMER("Initialization", "Image setup");
    LightManager::Initialize();
    PROFILER_END_TIMER("Initialization", "Lighting setup");
    ShaderLoader::Initialize();
    PROFILER_END_TIMER("Initialization", "Shader setup");
    MaterialLoader::Initialize();
    PROFILER_END_TIMER("Initialization", "Material setup");

    setupInputManager();
    PROFILER_END_TIMER("Initialization", "Input manager");

    glfwSetFramebufferSizeCallback(m_pWindow, [](GLFWwindow*, int nWidth, int nHeight) {
        Window::ins->onFramebufferSizeChanged(nWidth, nHeight);
    });

    GizmosManager::Initialize();
    PROFILER_END_TIMER("Initialization", "Gizmos setup");

    FontLoader::Initialize();
    FontLoader::getInstance()->loadFont("assets/fonts/arial.ttf");

    m_pRenderProcessQueue = new RenderProcessQueue(this);

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

    m_pImGuiEditorAddon = new ImGuiEditorAddon(this);
    PROFILER_END_TIMER("Initialization", "IMGui & editor setup");

#if IS_DEBUG_VERSION
    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_FUNCTION_1, [this](bool pressed) {
        if (pressed)
        {
            m_bShowDebugDepth = !m_bShowDebugDepth;
        }
    });

    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_FUNCTION_4, [](bool pressed) {
        if (pressed)
        {
            Preference::setForceOpenGLOnMac(!Preference::getForceOpenGLOnMac());
            Window::ins->sm_bRestartRequested = true;
        }
    });
#endif // IS_DEBUG_VERSION

    PROFILER_END_TIMER("Initialization", "Input key setup");

#if IS_DEBUG_VERSION
    m_pFileWatchDog = new FileWatchDog("assets/");
    m_pFileWatchDog->setFileChangeCallback([](const std::string& strFilePath, eFileChangeType eType) {
        EngineEventDispatcher::getInstance().runOnMainThread([strFilePath, eType]() {
            ShaderLoader::getInstance()->onFileChangedListener(strFilePath, eType);
            MaterialLoader::getInstance()->onFileChangedListener(strFilePath, eType);
        });
    });
    m_pFileWatchDog->startWatching();
#endif
}

#pragma endregion Managers and Game Engine Related Setup


#pragma region Window Setting Methods

void Window::setWindowSize(int nWidth, int nHeight)
{
    m_oWindowSize.x = nWidth;
    m_oWindowSize.y = nHeight;
    if (m_pWindow)
    {
        glfwSetWindowSize(m_pWindow, nWidth, nHeight);
    }
}

#pragma endregion Window Setting Methods


#pragma region GLFW Callbacks

void Window::onFramebufferSizeChanged(int nWidth, int nHeight)
{
    m_fRatio = nWidth / (float) nHeight;

    m_oActualSize.x = nWidth;
    m_oActualSize.y = nHeight;
    m_onWindowResize.invoke(m_oActualSize);

    glfwGetWindowSize(m_pWindow, &m_oWindowSize.x, &m_oWindowSize.y);

#if __APPLE__
    if (Renderer::isUsingMetal())
    {
        m_pMetalLayer->setDrawableSize(CGSizeMake(m_oActualSize.x, m_oActualSize.y));
        MetalRenderer::initMetalDepthTexture(m_pMetalDevice, m_oActualSize.x, m_oActualSize.y);
        MetalRenderer::initRenderPassDescriptor();
    }
#endif // __APPLE__
}

#pragma endregion GLFW Callbacks


#pragma region Main Loop and Frame Update

void Window::mainLoop()
{
    TimeManager::getInstance()->onWindowStart();

#if IS_DEBUG_VERSION
    while (!glfwWindowShouldClose(m_pWindow) && !sm_bRestartRequested)
#else
    while (!glfwWindowShouldClose(m_pWindow))
#endif
    {
        glfwPollEvents();
        runUpdate();

        if (Renderer::isUsingOpenGL())
        {
            glViewport(0, 0, m_oActualSize.x, m_oActualSize.y);
            glClearColor(0.f, 0.f, 0.f, m_bTransparentBackground ? 0 : 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            drawFrame();
            glfwSwapBuffers(m_pWindow);
        }
#if __APPLE__
        else if (Renderer::isUsingMetal())
        {
            NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

            m_pCurrentDrawable = m_pMetalLayer->nextDrawable();

            m_pCurrentCommandBuffer = m_pMetalCommandQueue->commandBuffer();

            drawFrame();

            m_pCurrentCommandBuffer->presentDrawable(m_pCurrentDrawable);
            m_pCurrentCommandBuffer->commit();

            pPool->release();
        }
#endif // __APPLE__
    }
}

void Window::runUpdate()
{
    float fDeltaTime = TimeManager::getInstance()->preUpdate();

    EngineEventDispatcher::getInstance().updateEvents(fDeltaTime);

    m_pWorldScene->update(fDeltaTime);

    if (Camera::main)
    {
        Camera::main->updateCameraDataBuffer();
    }

    LightManager::getInstance()->updateLightingUBO();
}


#if __APPLE__
void Window::setCurrentDrawingTexture(MTL::Texture* pTexture)
{
    if (m_pCurrentFrameRenderEncoder)
    {
        m_pCurrentFrameRenderEncoder->endEncoding();
    }

    MTL::RenderPassDescriptor* pRenderPassDescriptor = MetalRenderer::getRenderPassDescriptor();
    MTL::RenderPassColorAttachmentDescriptor* pColorAttachment = pRenderPassDescriptor->colorAttachments()->object(0);
    pColorAttachment->setTexture(pTexture);

    m_pCurrentFrameRenderEncoder = m_pCurrentCommandBuffer->renderCommandEncoder(pRenderPassDescriptor);
}
#endif // __APPLE__

void Window::drawFrame()
{
    m_nDrawCallCount = 0;
    m_nTriangleCount = 0;

    LightManager* const pLightManager = LightManager::getInstance();
    DirectionLightComponent* pMainDirLight = LightManager::getInstance()->getMainDirectionLightComponent();
    if (pMainDirLight && pMainDirLight->getShadowsEnabled())
    {
        if (Renderer::isUsingOpenGL())
        {
            glViewport(0, 0, LightManager::SHADOW_MAP_WIDTH, LightManager::SHADOW_MAP_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, pLightManager->getShadowDepthMapFBO());
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            m_pWorldScene->renderDepth();
            glCullFace(GL_BACK);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
#if __APPLE__
        else if (Renderer::isUsingMetal())
        {
            MetalRenderer::startNewDepthOnlyFrame(pLightManager->getShadowDepthMapTextureMetal());
            m_pWorldScene->renderDepth();
            MetalRenderer::endDepthOnlyFrame();
        }
#endif // __APPLE__
    }

    if (m_bShowDebugDepth && Renderer::isUsingMetal())
    {
#if __APPLE__
        setCurrentDrawingTexture(m_pCurrentDrawable->texture());

        Shader* pDebugDepthShader = ShaderLoader::getInstance()->getShader("depth_debug");
        m_pCurrentFrameRenderEncoder->setRenderPipelineState(pDebugDepthShader->getMetalPipelineState());
        m_pCurrentFrameRenderEncoder->setVertexBuffer(m_pRenderProcessQueue->getMetalFullScreenVertexBuffer(), 0, 0);
        m_pCurrentFrameRenderEncoder->setFragmentTexture(LightManager::getInstance()->getShadowDepthMapTextureMetal(), 0);
        m_pCurrentFrameRenderEncoder->setFragmentSamplerState(MetalRenderer::m_pLinearSampler, 0);

        m_pCurrentFrameRenderEncoder->drawPrimitives(MTL::PrimitiveTypeTriangleStrip, NS::UInteger(0), NS::UInteger(4));
#endif // __APPLE__
    }
    else
    {
    if (m_bEnablePostProcess) // Enable post process
    {
        m_pRenderProcessQueue->beginFrame();
        m_pWorldScene->render();
        m_pRenderProcessQueue->endFrame();

        if (Renderer::isUsingOpenGL())
        {
            glDisable(GL_DEPTH_TEST);
        }
        m_pRenderProcessQueue->startProcessing();
        m_pRenderProcessQueue->renderToScreen();
        if (Renderer::isUsingOpenGL())
        {
            glEnable(GL_DEPTH_TEST);
        }
    }
    else
    {
#if __APPLE__
        if (Renderer::isUsingMetal())
        {
        setCurrentDrawingTexture(m_pCurrentDrawable->texture());
        }
#endif // __APPLE__

        m_pWorldScene->render();
    }
    }

    if (m_bDrawGizmos)
    {
        m_pWorldScene->drawGizmos();
    }

#if __APPLE__
    if (Renderer::isUsingMetal())
    {
        m_pCurrentFrameRenderEncoder->endEncoding();
        m_pCurrentFrameRenderEncoder = nullptr;
    }
#endif // __APPLE__

    if (m_pImGuiEditorAddon)
    {
        m_pImGuiEditorAddon->startIMGUIFrame();
        m_pImGuiEditorAddon->update();
        m_pImGuiEditorAddon->renderFrame();
    }
}

#pragma endregion Main Loop and Frame Update

#pragma once

#include "debug_macro.h"
#include "input/event.h"
#include "math/vector.h"
#include "../../utils/expandable_array.h"
#include "../../editor/editor_window.h"

#if __APPLE__
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#endif // __APPLE__

typedef struct GLFWwindow GLFWwindow;

class WorldScene;
class RenderProcessQueue;
class FileWatchDog;

typedef unsigned int GLuint;

enum class GraphicAPI
{
    OpenGL,
    Metal
};

class Window {
public:
    static Window* ins;

#if IS_DEBUG_VERSION
    static inline bool sm_bRestartRequested = false;
#endif // IS_DEBUG_VERSION

    Window();
    ~Window();

    inline void setResizable(bool resizable) { m_bResizable = resizable; }
    inline void setKeepRatio(bool keepRatio) { m_bKeepRatio = keepRatio; }
    inline float getWindowRatio() { return m_fRatio; }

    inline const Vector2i& getActualSize() const { return m_oActualSize; }
    inline int GetActualWidth() const { return m_oActualSize.x; }
    inline int GetActualHeight() const { return m_oActualSize.y; }

    void setWindowSize(int nWidth, int nHeight);
    inline const Vector2i& getWindowSize() const { return m_oWindowSize; }

    inline bool isPostProcessEnabled() const { return m_bEnablePostProcess; }
    inline void setPostProcessEnabled(bool enabled) { m_bEnablePostProcess = enabled; }

    inline RenderProcessQueue* getRenderProcessQueue() const { return m_pRenderProcessQueue; }

    inline void setAddGameRelatedIMGUIWindows(bool bAdd) { m_bAddGameRelatedIMGUIWindows = bAdd; }
    inline void addEditorWindow(IEditorWindow* pWindow) { m_oEditorWindows.addElement(pWindow); }

    inline void setShowFPS(bool bShow) { m_bShowFPS = bShow; }

    bool configureAndCreateWindow();
    void configureGLFWWithOpenGL();
    void configureGLFWWithMetal();

    void initializeGraphicsAPI();

    void setupManagers();
    void setupGameEngineRelatedObject();

    void mainLoop();

    inline void increaseDrawCallCount() { ++m_nDrawCallCount; }
    inline void increaseDrawCallCount(int nTriangleCount) { ++m_nDrawCallCount; m_nTriangleCount += nTriangleCount; }

    inline void registerResizeListener(std::function<void(Vector2i)> funcListener) 
    { 
        m_onWindowResize.add(funcListener); 
    }

#if __APPLE__
    inline MTL::Device* getMetalDevice() const { return m_pMetalDevice; }

    inline CA::MetalDrawable* getCurrentDrawable() const { return m_pCurrentDrawable; }
    inline MTL::RenderCommandEncoder* getCurrentFrameRenderEncoder() const { return m_pCurrentFrameRenderEncoder; }
    inline MTL::RenderCommandEncoder* getCurrentFrameDepthRenderEncoder() const { return m_pCurrentFrameDepthRenderEncoder; }

    void setCurrentDrawingTexture(MTL::Texture* pTexture);
#endif // __APPLE__

    inline GraphicAPI getGraphicAPI() const { return m_eGraphicAPI; }
    inline bool isUsingMetal() const { return m_eGraphicAPI == GraphicAPI::Metal; }
    inline bool isUsingOpenGL() const { return m_eGraphicAPI == GraphicAPI::OpenGL; }

private:
    GLFWwindow* m_pWindow = nullptr;

    GraphicAPI m_eGraphicAPI = GraphicAPI::OpenGL;

#if __APPLE__
    MTL::Device* m_pMetalDevice = nullptr;
    CA::MetalLayer* m_pMetalLayer = nullptr;

    MTL::CommandQueue* m_pMetalCommandQueue = nullptr;
    MTL::RenderPassDescriptor* m_pRenderPassDescriptor = nullptr;
    MTL::RenderPassDescriptor* m_pDepthOnlyRenderPassDescriptor = nullptr;

    MTL::CommandBuffer* m_pCurrentCommandBuffer = nullptr;
    MTL::RenderCommandEncoder* m_pCurrentFrameRenderEncoder = nullptr;
    MTL::RenderCommandEncoder* m_pCurrentFrameDepthRenderEncoder = nullptr;
    CA::MetalDrawable* m_pCurrentDrawable = nullptr;
#endif // __APPLE__

    RenderProcessQueue* m_pRenderProcessQueue = nullptr;
    bool m_bEnablePostProcess = true;

    Vector2i m_oWindowSize;
    Vector2i m_oActualSize;
    float m_fRatio = 1.0f;

    // double m_fCurrentDrawTime = 0.0;
    // double m_fLastDrawTime = 0.0;
    // float m_fDeltaTime = 0.0;

    WorldScene* m_pWorldScene = nullptr;

    bool m_bResizable = false;
    bool m_bKeepRatio = false;
    int m_nDrawCallCount = 0;
    int m_nTriangleCount = 0;

    bool m_bShowIMGUI = false;
    bool m_bAddGameRelatedIMGUIWindows = true;
    bool m_bShowFPS = true;
    PointerExpandableArray<IEditorWindow*> m_oEditorWindows = PointerExpandableArray<IEditorWindow*>(2);

    FileWatchDog* m_pFileWatchDog = nullptr;

    CustomEvent<Vector2i> m_onWindowResize;

    bool m_bShowDebugDepth = false;

#if IS_DEBUG_VERSION
    bool m_bDrawGizmos = true;
#else
    bool m_bDrawGizmos = false;
#endif // IS_DEBUG_VERSION

    void bindOpenGLToGlfwWindow();
#if __APPLE__
    void bindMetalToGlfwWindow();
#endif // __APPLE__

    void setupInputManager();

    void setupIMGUIAndEditorWindows();

    void beforeLoop();
    void IMGUINewFrame();
    void runUpdate();
    void drawIMGUIEditor();
    void drawFrame();
    void drawFrameInfo();
};

#if IS_DEBUG_VERSION
#define INCREASE_DRAW_CALL_COUNT(n) Window::ins->increaseDrawCallCount(n)
#else
#define INCREASE_DRAW_CALL_COUNT(n) do {} while (0)
#endif // IS_DEBUG_VERSION
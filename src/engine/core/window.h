#pragma once

#include "debug_macro.h"
#include "input/event.h"
#include "math/vector.h"
#include "../../utils/expandable_array.h"

#if __APPLE__
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#endif // __APPLE__

typedef struct GLFWwindow GLFWwindow;

class WorldScene;
class RenderProcessQueue;
class FileWatchDog;
class ImGuiEditorAddon;

typedef unsigned int GLuint;

class Window {
public:
    static Window* ins;

#if IS_DEBUG_VERSION
    static inline bool sm_bRestartRequested = false;
#endif // IS_DEBUG_VERSION

    Window();
    ~Window();

    inline GLFWwindow* getGLFWwindow() const { return m_pWindow; }
    inline void setResizable(bool resizable) { m_bResizable = resizable; }
    inline void setKeepRatio(bool keepRatio) { m_bKeepRatio = keepRatio; }
    inline void setTransparentBackground(bool transparent) { m_bTransparentBackground = transparent; }
    inline bool getTransparentBackground() const { return m_bTransparentBackground; }
    inline float getWindowRatio() { return m_fRatio; }

    inline const Vector2i& getActualSize() const { return m_oActualSize; }
    inline int GetActualWidth() const { return m_oActualSize.x; }
    inline int GetActualHeight() const { return m_oActualSize.y; }

    void setWindowSize(int nWidth, int nHeight);
    inline const Vector2i& getWindowSize() const { return m_oWindowSize; }

    inline bool isPostProcessEnabled() const { return m_bEnablePostProcess; }
    inline void setPostProcessEnabled(bool enabled) { m_bEnablePostProcess = enabled; }

    inline RenderProcessQueue* getRenderProcessQueue() const { return m_pRenderProcessQueue; }

    inline ImGuiEditorAddon* getImGuiEditorAddon() const { return m_pImGuiEditorAddon; }

    bool configureAndCreateWindow();

    void initializeGraphicsAPI();

    void setupManagers();
    void setupGameEngineRelatedObject();

    void mainLoop();

    inline void registerResizeListener(std::function<void(Vector2i)> funcListener) 
    { 
        m_onWindowResize.add(funcListener); 
    }

    inline bool getDrawGizmos() const { return m_bDrawGizmos; }
    inline void setDrawGizmos(bool drawGizmos) { m_bDrawGizmos = drawGizmos; }

#if __APPLE__
    inline MTL::Device* getMetalDevice() const { return m_pMetalDevice; }

    inline CA::MetalDrawable* getCurrentDrawable() const { return m_pCurrentDrawable; }
    inline MTL::CommandBuffer* getCurrentCommandBuffer() const { return m_pCurrentCommandBuffer; }
    inline MTL::RenderCommandEncoder* getCurrentFrameRenderEncoder() const { return m_pCurrentFrameRenderEncoder; }

    void setCurrentDrawingTexture(MTL::Texture* pTexture);
#endif // __APPLE__


private:
    GLFWwindow* m_pWindow = nullptr;

#if __APPLE__
    MTL::Device* m_pMetalDevice = nullptr;
    CA::MetalLayer* m_pMetalLayer = nullptr;

    MTL::CommandQueue* m_pMetalCommandQueue = nullptr;

    MTL::CommandBuffer* m_pCurrentCommandBuffer = nullptr;
    MTL::RenderCommandEncoder* m_pCurrentFrameRenderEncoder = nullptr;
    CA::MetalDrawable* m_pCurrentDrawable = nullptr;
#endif // __APPLE__

    RenderProcessQueue* m_pRenderProcessQueue = nullptr;
    bool m_bEnablePostProcess = true;

    Vector2i m_oWindowSize;
    Vector2i m_oActualSize;
    float m_fRatio = 1.0f;

    WorldScene* m_pWorldScene = nullptr;

    // GLFW Window Configuration
    bool m_bResizable = false;
    bool m_bKeepRatio = false;
    bool m_bTransparentBackground = false;

    int m_nDrawCallCount = 0;
    int m_nTriangleCount = 0;

    bool m_bAddGameRelatedIMGUIWindows = true;

    FileWatchDog* m_pFileWatchDog = nullptr;

    CustomEvent<Vector2i> m_onWindowResize;

    ImGuiEditorAddon* m_pImGuiEditorAddon = nullptr;

    bool m_bShowDebugDepth = false;

#if IS_DEBUG_VERSION
    bool m_bDrawGizmos = true;
#else
    bool m_bDrawGizmos = false;
#endif // IS_DEBUG_VERSION

    void configureGLFWBeforeWindowCreation();

    void bindOpenGLToGlfwWindow();
#if __APPLE__
    void bindMetalToGlfwWindow();
#endif // __APPLE__

    void setupInputManager();

    void beforeLoop();
    void IMGUINewFrame();
    void runUpdate();
    void drawIMGUIEditor();
    void drawFrame();
};

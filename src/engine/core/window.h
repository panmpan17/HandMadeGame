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
// class RenderProcessQueue;
// class IEditorWindow;
class FileWatchDog;

typedef unsigned int GLuint;

class Window {
public:
    static Window* ins;

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

    // inline RenderProcessQueue* getRenderProcessQueue() const { return m_pRenderProcessQueue; }

    inline void setAddGameRelatedIMGUIWindows(bool bAdd) { m_bAddGameRelatedIMGUIWindows = bAdd; }
    // inline void addEditorWindow(IEditorWindow* pWindow) { m_oEditorWindows.addElement(pWindow); }

    inline void setShowFPS(bool bShow) { m_bShowFPS = bShow; }

    bool configureAndCreateWindow();

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

    inline MTL::Device* getMetalDevice() const { return m_pMetalDevice; }

private:
    GLFWwindow* m_pWindow = nullptr;

#if __APPLE__
    MTL::Device* m_pMetalDevice = nullptr;
    CA::MetalLayer* m_pMetalLayer = nullptr;
#endif // __APPLE__

    // RenderProcessQueue* m_pRenderProcessQueue = nullptr;
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
    // PointerExpandableArray<IEditorWindow*> m_oEditorWindows = PointerExpandableArray<IEditorWindow*>(2);

    FileWatchDog* m_pFileWatchDog = nullptr;

    CustomEvent<Vector2i> m_onWindowResize;

#if IS_DEBUG_VERSION
    bool m_bDrawGizmos = true;
#else
    bool m_bDrawGizmos = false;
#endif // IS_DEBUG_VERSION

    void bindOpenGLToGlfwWindow();
    void bindMetalToGlfwWindow();

    void setupInputManager();

    void setupIMGUIAndEditorWindows();

    void beforeLoop();
    void runUpdate();
    void updateIMGUI();
    void drawFrame();
    void drawFrameInfo();
};

#if IS_DEBUG_VERSION
#define INCREASE_DRAW_CALL_COUNT(n) Window::ins->increaseDrawCallCount(n)
#else
#define INCREASE_DRAW_CALL_COUNT(n) do {} while (0)
#endif // IS_DEBUG_VERSION
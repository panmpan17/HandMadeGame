#pragma once

#include "debug_macro.h"
#include "../../utils/expandable_array.h"

typedef struct GLFWwindow GLFWwindow;

class WorldScene;
class RenderProcessQueue;
class IEditorWindow;

typedef unsigned int GLuint;

class Window {
public:
    static Window* ins;

    Window();
    ~Window();

    inline bool isValid() const { return m_pWindow != nullptr; }

    inline void setResizable(bool resizable) { m_bResizable = resizable; }
    inline float getWindowRatio() { return m_fRatio; }
    inline void getWindowSize(int& width, int& height) {
        width = m_nWidth;
        height = m_nHeight;
    }

    inline int GetActualWidth() const { return m_nActualWidth; }
    inline int GetActualHeight() const { return m_nActualHeight; }

    inline bool isPostProcessEnabled() const { return m_bEnablePostProcess; }
    inline void setPostProcessEnabled(bool enabled) { m_bEnablePostProcess = enabled; }

    void configureAndCreateWindow();
    void setupManagers();

    void mainLoop();

    void increaseDrawCallCount();

private:
    GLFWwindow* m_pWindow = nullptr;
    RenderProcessQueue* m_pRenderProcessQueue = nullptr;
    bool m_bEnablePostProcess = true;

    int m_nWidth = 1920, m_nHeight = 1080;
    int m_nActualWidth = 800, m_nActualHeight = 600;
    float m_fRatio = 1.0f;

    double m_fCurrentDrawTime = 0.0;
    double m_fLastDrawTime = 0.0;
    float m_fDeltaTime = 0.0;

    WorldScene* m_pWorldScene = nullptr;

    bool m_bResizable = false;
    int m_nDrawCallCount = 0;

    bool m_bShowIMGUI = false;
    PointerExpandableArray<IEditorWindow*> m_oEditorWindows = PointerExpandableArray<IEditorWindow*>(2);

    void beforeLoop();
    void mainLoop_IMGUI();
    void drawFrame();
    void drawFrameInfo();
};

#if IS_DEBUG_VERSION
#define INCREASE_DRAW_CALL_COUNT() Window::ins->increaseDrawCallCount()
#else
#define INCREASE_DRAW_CALL_COUNT() do {} while (0)
#endif

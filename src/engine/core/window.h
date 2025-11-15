#pragma once

#include "debug_macro.h"
#include "input/event.h"
#include "math/vector.h"
#include "../../utils/expandable_array.h"

typedef struct GLFWwindow GLFWwindow;

class WorldScene;
class RenderProcessQueue;
class IEditorWindow;
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

    inline Vector2i& getActualSize() { return m_oActualSize; }
    inline int GetActualWidth() const { return m_oActualSize.x; }
    inline int GetActualHeight() const { return m_oActualSize.y; }

    inline bool isPostProcessEnabled() const { return m_bEnablePostProcess; }
    inline void setPostProcessEnabled(bool enabled) { m_bEnablePostProcess = enabled; }

    inline RenderProcessQueue* getRenderProcessQueue() const { return m_pRenderProcessQueue; }

    bool configureAndCreateWindow();
    void setupManagers();

    void mainLoop();

    inline void increaseDrawCallCount() { ++m_nDrawCallCount; }

    inline void registerResizeListener(std::function<void(Vector2i)> funcListener) 
    { 
        m_onWindowResize.add(funcListener); 
    }

private:
    GLFWwindow* m_pWindow = nullptr;
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

    bool m_bShowIMGUI = false;
    PointerExpandableArray<IEditorWindow*> m_oEditorWindows = PointerExpandableArray<IEditorWindow*>(2);

    FileWatchDog* m_pFileWatchDog = nullptr;

    Event<Vector2i> m_onWindowResize;

    void setupInputManager();
    void setupIMGUIAndEditorWindows();

    void beforeLoop();
    void runUpdate();
    void updateIMGUI();
    void drawFrame();
    void drawFrameInfo();
};

#if IS_DEBUG_VERSION
#define INCREASE_DRAW_CALL_COUNT() Window::ins->increaseDrawCallCount()
#else
#define INCREASE_DRAW_CALL_COUNT() do {} while (0)
#endif

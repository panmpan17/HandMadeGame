#pragma once

#include "debug_macro.h"

typedef struct GLFWwindow GLFWwindow;
class Camera;
class WorldScene;

typedef unsigned int GLuint;

class BloomTest;
class RenderProcessQueue;

class Window {
public:
    static Window* ins;

    Window();
    ~Window();

    inline bool isValid() const { return m_pWindow != nullptr; }

    inline void setResizable(bool resizable) { m_bResizable = resizable; }
    inline float getWindowRatio() { return ins->m_fRatio; }
    inline void getWindowSize(int& width, int& height) {
        width = ins->m_nWidth;
        height = ins->m_nHeight;
    }

    inline int GetActualWidth() const { return m_nActualWidth; }
    inline int GetActualHeight() const { return m_nActualHeight; }

    void configureAndCreateWindow();
    void start();

    void increaseDrawCallCount();

private:
    BloomTest* m_pBloomTest = nullptr;
    GLFWwindow* m_pWindow = nullptr;
    RenderProcessQueue* m_pRenderProcessQueue = nullptr;

    int m_nWidth = 320, m_nHeight = 180;
    int m_nActualWidth = 800, m_nActualHeight = 600;
    float m_fRatio = 1.0f;

    double m_fCurrentDrawTime = 0.0;
    double m_fLastDrawTime = 0.0;
    float m_fDeltaTime = 0.0;

    Camera* m_pCamera = nullptr;

    WorldScene* m_pWorldScene = nullptr;

    bool m_bResizable = false;
    int m_nDrawCallCount = 0;

    void mainLoop();
    void drawFrame();
};

#if IS_DEBUG_VERSION
#define INCREASE_DRAW_CALL_COUNT() Window::ins->increaseDrawCallCount()
#else
#define INCREASE_DRAW_CALL_COUNT() do {} while (0)
#endif

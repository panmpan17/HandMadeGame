#pragma once

#include "debug_macro.h"

typedef struct GLFWwindow GLFWwindow;
class Camera;
class WorldScene;

typedef unsigned int GLuint;

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

    void configureAndCreateWindow();
    void start();

    void increaseDrawCallCount();

private:
    GLFWwindow* m_pWindow = nullptr;

    int m_nWidth = 800, m_nHeight = 450;
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

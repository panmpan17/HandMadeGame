#pragma once

#include "debug_macro.h"

class GLFWwindow;
class Camera;
class TestShader;
class ImageShader;
class Image;
class Node;

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

    void addNode(Node* pNode);

    void increaseDrawCallCount();

private:
    GLFWwindow* m_pWindow = nullptr;

    TestShader* m_pBaseShader = nullptr;
    ImageShader* m_pImageShader = nullptr;

    int m_nWidth = 80, m_nHeight = 60;
    int m_nActualWidth = 800, m_nActualHeight = 600;
    float m_fRatio = 1.0f;

    double m_fCurrentDrawTime = 0.0;
    double m_fLastDrawTime = 0.0;
    float m_fDeltaTime = 0.0;

    Camera* m_pCamera = nullptr;

    // TODO: make this extensible
    int m_nNodeCount = 10;
    Node* m_pNodes[10] = { nullptr };

    bool m_bResizable = false;
    int m_nDrawCallCount = 0;

    void setupGLVertex();
    void setupShaders();
    void mainLoop();
    void drawFrame();
};

#if IS_DEBUG_VERSION
#define INCREASE_DRAW_CALL_COUNT() Window::ins->increaseDrawCallCount()
#else
#define INCREASE_DRAW_CALL_COUNT() do {} while (0)
#endif

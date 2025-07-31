#pragma once

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
    inline static float getWindowRatio() { return ins->m_fRatio; }
    inline static void getWindowSize(int& width, int& height) {
        width = ins->m_nWidth;
        height = ins->m_nHeight;
    }

    Window();
    ~Window();

    inline bool isValid() const { return m_pWindow != nullptr; }

    void start();

    void addNode(Node* pNode);

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

    // TODO: optimization, load it some where centralized
    Image* m_pImage = nullptr;

    void configureAndCreateWindow();
    void setupGLVertex();
    void setupShaders();
    void mainLoop();
    void drawFrame();
};
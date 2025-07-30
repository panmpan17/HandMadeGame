#pragma once

class GLFWwindow;
class Camera;
class TestShader;
class ImageShader;
class Quad;
class Triangle;
class IDrawable;
class Image;

typedef unsigned int GLuint;

class Window {
public:
    static Window* ins;
    static float getWindowRatio() { return ins->m_fRatio; }
    static void getWindowSize(int& width, int& height) {
        width = ins->m_nWidth;
        height = ins->m_nHeight;
    }

    Window();
    ~Window();

    bool isValid() const { return m_pWindow != nullptr; }

    void start();

private:
    GLFWwindow* m_pWindow = nullptr;

    TestShader* m_pBaseShader = nullptr;
    ImageShader* m_pImageShader = nullptr;

    int m_nWidth = 800, m_nHeight = 600;
    int m_nActualWidth = 800, m_nActualHeight = 600;
    float m_fRatio = 1.0f;

    double m_fCurrentDrawTime = 0.0;
    double m_fLastDrawTime = 0.0;
    float m_fDeltaTime = 0.0;

    Camera* m_pCamera = nullptr;

    IDrawable** m_pDrawables = nullptr;
    int m_nDrawableCount = 0;

    // TODO: optimization, load it some where centralized
    Image* m_pImage = nullptr;

    void configureAndCreateWindow();
    void setupGLVertex();
    void setupShaders();
    void mainLoop();
    void drawFrame();
};
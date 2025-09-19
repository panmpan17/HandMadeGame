#pragma once

typedef unsigned int GLuint;

class Shader;
class Window;

class RenderProcessQueue
{
public:
    RenderProcessQueue(Window* pWindow);
    ~RenderProcessQueue();

    // void resize(int width, int height);

    void beginFrame();
    void endFrame();

    void startProcessing();
    void renderToScreen();

private:
    Window* m_pWindow = nullptr;

    int m_nRenderWidth = 0;
    int m_nRenderHeight = 0;

    GLuint m_nFBOID_original = 0;
    GLuint m_nRenderTexture_original = 0;

    Shader* m_pShader = nullptr;
    GLuint m_nTextureUniform = 0;
    GLuint m_nVertexBuffer = 0;
    GLuint m_nVertexArray = 0;

    GLuint m_nFinalRenderTexture = 0;

    void init(int nWidth, int nHeight);
    void initializeQuad();
    void initializeOriginalFBO();
};
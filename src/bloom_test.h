#pragma once


class Window;
class Shader;


typedef unsigned int GLuint;

class BloomTest
{
public:
    BloomTest() {}

    void initialize(const Window* pWindow);
    void startRenderingFBO(const Window* pWindow);
    void endRenderingFBO(const Window* pWindow);

private:
    GLuint m_nFBOID;
    GLuint m_nRenderTexture;
    GLuint m_nVertexBuffer;
    GLuint m_nVertexArray;
    GLuint m_nTextureUniform;

    int m_nRenderWidth, m_nRenderHeight;

    Shader* m_pShader = nullptr;

    void initializeColorHighlightFBO(const Window* pWindow);
    void initializeQuad();
};
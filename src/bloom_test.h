#pragma once


class Window;
class Shader;


typedef unsigned int GLuint;

class BloomTest
{
public:
    BloomTest() {}

    void initialize(const Window* pWindow);
    void startRenderingGame(const Window* pWindow);
    void endRenderingGame(const Window* pWindow);

private:
    GLuint m_nFBOID_ColorHighlight = 0;
    GLuint m_nRenderTexture_ColorHighlight = 0;

    GLuint m_nFBOID_HorizontalBlur = 0;
    GLuint m_nRenderTexture_HorizontalBlur = 0;

    GLuint m_nFBOID_VerticalBlur = 0;
    GLuint m_nRenderTexture_VerticalBlur = 0;

    GLuint m_nVertexBuffer;
    GLuint m_nVertexArray;

    int m_nRenderWidth, m_nRenderHeight;

    Shader* m_pColorHighlightShader = nullptr;
    GLuint m_nTextureUniform_ColorHighlight;

    Shader* m_pHorizontalBlurShader = nullptr;
    GLuint m_nTextureUniform_HorizontalBlur;
    GLuint m_nTextureWidthUniform_HorizontalBlur;

    Shader* m_pVerticalBlurShader = nullptr;
    GLuint m_nTextureUniform_VerticalBlur;
    GLuint m_nTextureHeightUniform_VerticalBlur;

    void initializeColorHighlightFBO();
    void initializeHorizontalBlurFBO();
    void initializeVerticalBlurFBO();

    void initializeQuad();

    void renderColorHighlight(const Window* pWindow);
    void renderHorizontalBlur(const Window* pWindow);
    void renderVerticalBlur(const Window* pWindow);
};
#pragma once

#include "render_process_queue.h"

class Shader;


typedef unsigned int GLuint;

class BloomTest : public IRenderProcess
{
public:
    BloomTest(RenderProcessQueue* pQueue) : IRenderProcess(pQueue) {}

    void initialize() override;

    void renderProcess() override;

private:
    GLuint m_nOriginalRenderTexture = 0;

    GLuint m_nFBOID_ColorHighlight = 0;
    GLuint m_nRenderTexture_ColorHighlight = 0;

    GLuint m_nFBOID_HorizontalBlur = 0;
    GLuint m_nRenderTexture_HorizontalBlur = 0;

    GLuint m_nFBOID_VerticalBlur = 0;
    GLuint m_nRenderTexture_VerticalBlur = 0;

    GLuint m_nFBOID_Final = 0;
    GLuint m_nRenderTexture_Final = 0;

    int m_nRenderWidth, m_nRenderHeight;

    Shader* m_pColorHighlightShader = nullptr;
    GLuint m_nTextureUniform_ColorHighlight;

    Shader* m_pHorizontalBlurShader = nullptr;
    GLuint m_nTextureUniform_HorizontalBlur;
    GLuint m_nTextureWidthUniform_HorizontalBlur;

    Shader* m_pVerticalBlurShader = nullptr;
    GLuint m_nTextureUniform_VerticalBlur;
    GLuint m_nTextureHeightUniform_VerticalBlur;

    Shader* m_pCompositeShader = nullptr;
    GLuint m_nOriginalTextureUniform;
    GLuint m_nBloomTextureUniform;
    GLuint m_nBloomTextureScaleUniform;
    GLuint m_nIntensityUniform;

    float m_nIntensity = 1.0f;

    void initializeColorHighlightFBO();
    void initializeHorizontalBlurFBO();
    void initializeVerticalBlurFBO();
    void initializeFinalFBO();

    void initializeQuad();

    void renderColorHighlight();
    void renderHorizontalBlur();
    void renderVerticalBlur();
    void renderComposite();
};
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

    inline void setIntensity(float intensity) { m_nIntensity = intensity; }
    inline float getIntensity() const { return m_nIntensity; }

    inline void setBlurRadius(float radius) { m_fBlurRadius = radius; }
    inline float getBlurRadius() const { return m_fBlurRadius; }

    inline void setBlurSigma(float sigma) { m_fBlurSigma = sigma; }
    inline float getBlurSigma() const { return m_fBlurSigma; }

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
    GLuint m_nBlurRadiusUniform_HorizontalBlur;
    GLuint m_nBlurSigmaUniform_HorizontalBlur;

    Shader* m_pVerticalBlurShader = nullptr;
    GLuint m_nTextureUniform_VerticalBlur;
    GLuint m_nTextureHeightUniform_VerticalBlur;
    GLuint m_nBlurRadiusUniform_VerticalBlur;
    GLuint m_nBlurSigmaUniform_VerticalBlur;

    Shader* m_pCompositeShader = nullptr;
    GLuint m_nOriginalTextureUniform;
    GLuint m_nBloomTextureUniform;
    GLuint m_nBloomTextureScaleUniform;
    GLuint m_nIntensityUniform;

    float m_nIntensity = 1.0f;
    float m_fBlurRadius = 4.f; // 0 to 20
    float m_fBlurSigma = 4.f;  // 0 to 20

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
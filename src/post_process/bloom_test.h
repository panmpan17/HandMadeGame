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

    inline void setHighlightThreshold(float threshold) { m_fHighlightThreshold = threshold; }
    inline float getHighlightThreshold() const { return m_fHighlightThreshold; }

    inline void setIntensity(float intensity) { m_nIntensity = intensity; }
    inline float getIntensity() const { return m_nIntensity; }

    inline void setBlurRadius(float radius) { m_fBlurRadius = radius; }
    inline float getBlurRadius() const { return m_fBlurRadius; }

    inline void setBlurSigma(float sigma) { m_fBlurSigma = sigma; }
    inline float getBlurSigma() const { return m_fBlurSigma; }

    inline int getBloomProcessDebugStep() const { return m_nBloomProcessDebugStep; }
    inline void setBloomProcessDebugStep(int step) { m_nBloomProcessDebugStep = step; }

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
    const ShaderUniformHandle* m_pTextureUniform_ColorHighlight = nullptr;
    const ShaderUniformHandle* m_pThresholdUniform_ColorHighlight = nullptr;

    Shader* m_pHorizontalBlurShader = nullptr;
    const ShaderUniformHandle* m_pTextureUniform_HorizontalBlur = nullptr;
    const ShaderUniformHandle* m_pTextureWidthUniform_HorizontalBlur = nullptr;
    const ShaderUniformHandle* m_pBlurRadiusUniform_HorizontalBlur = nullptr;
    const ShaderUniformHandle* m_pBlurSigmaUniform_HorizontalBlur = nullptr;

    Shader* m_pVerticalBlurShader = nullptr;
    const ShaderUniformHandle* m_pTextureUniform_VerticalBlur = nullptr;
    const ShaderUniformHandle* m_pTextureHeightUniform_VerticalBlur = nullptr;
    const ShaderUniformHandle* m_pBlurRadiusUniform_VerticalBlur = nullptr;
    const ShaderUniformHandle* m_pBlurSigmaUniform_VerticalBlur = nullptr;

    Shader* m_pCompositeShader = nullptr;
    const ShaderUniformHandle* m_pOriginalTextureUniform = nullptr;
    const ShaderUniformHandle* m_pBloomTextureUniform = nullptr;
    const ShaderUniformHandle* m_pBloomTextureScaleUniform = nullptr;
    const ShaderUniformHandle* m_pIntensityUniform = nullptr;

    float m_fHighlightThreshold = 0.8f;
    float m_nIntensity = 1.0f;
    float m_fBlurRadius = 4.f; // 0 to 20
    float m_fBlurSigma = 4.f;  // 0 to 20

    int m_nBloomProcessDebugStep = 4;

    void initializeQuad();

    void renderColorHighlight();
    void renderHorizontalBlur();
    void renderVerticalBlur();
    void renderComposite();
};

REGISTER_CLASS_NO_CREATOR(BloomTest);

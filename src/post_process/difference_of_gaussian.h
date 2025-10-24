#pragma once

#include "render_process_queue.h"


class Shader;
typedef unsigned int GLuint;


class DifferenceOfGaussian : public IRenderProcess
{
public:
    DifferenceOfGaussian(RenderProcessQueue* pQueue) : IRenderProcess(pQueue) {}

    void initialize() override;

    void renderProcess() override;

private:
    GLuint m_nOriginalRenderTexture = 0;

    GLuint m_nFBOID_Blur1_Horizontal = 0;
    GLuint m_nRenderTexture_Blur1_Horizontal = 0;

    GLuint m_nFBOID_BLur1_Vertical = 0;
    GLuint m_nRenderTexture_Blur1_Vertical = 0;

    GLuint m_nFBOID_Blur2_Horizontal = 0;
    GLuint m_nRenderTexture_Blur2_Horizontal = 0;

    GLuint m_nFBOID_BLur2_Vertical = 0;
    GLuint m_nRenderTexture_Blur2_Vertical = 0;

    GLuint m_nFBOID_Final = 0;
    GLuint m_nRenderTexture_Final = 0;

    int m_nRenderWidth, m_nRenderHeight;

    float m_fBlurRadius1 = 2.0f;
    float m_fBlurSigma1 = 2.0f;

    float m_fBlurRadius2 = 4.0f;
    float m_fBlurSigma2 = 4.0f;

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
    const ShaderUniformHandle* m_pBlur1TextureUniform = nullptr;
    const ShaderUniformHandle* m_pBlur2TextureUniform = nullptr;

    void initializeQuad();

    void renderBlur1Horizontal();
    void renderBlur1Vertical();

    void renderBlur2Horizontal();
    void renderBlur2Vertical();

    void renderComposite();
};

REGISTER_CLASS_NO_CREATOR(DifferenceOfGaussian);

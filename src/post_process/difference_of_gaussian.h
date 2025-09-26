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
    GLuint m_nBlur1TextureUniform;
    GLuint m_nBlur2TextureUniform;

    void initializeQuad();

    void renderBlur1Horizontal();
    void renderBlur1Vertical();

    void renderBlur2Horizontal();
    void renderBlur2Vertical();

    void renderComposite();
};

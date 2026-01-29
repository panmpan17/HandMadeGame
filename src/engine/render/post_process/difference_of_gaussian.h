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

    void onWindowResize() override;

    inline float getBlurRadius1() const { return m_fBlurRadius1; }
    inline void setBlurRadius1(float fRadius) { m_fBlurRadius1 = fRadius; }

    inline float getBlurSigma1() const { return m_fBlurSigma1; }
    inline void setBlurSigma1(float fSigma) { m_fBlurSigma1 = fSigma; }

    inline float getBlurRadius2() const { return m_fBlurRadius2; }
    inline void setBlurRadius2(float fRadius) { m_fBlurRadius2 = fRadius; }

    inline float getBlurSigma2() const { return m_fBlurSigma2; }
    inline void setBlurSigma2(float fSigma) { m_fBlurSigma2 = fSigma; }

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

#if __APPLE__
    MTL::Texture* m_pOriginalRenderTexture_Metal = nullptr;
    MTL::Texture* m_pRenderTexture_Blur1_Horizontal_Metal = nullptr;
    MTL::Texture* m_pRenderTexture_Blur1_Vertical_Metal = nullptr;
    MTL::Texture* m_pRenderTexture_Blur2_Horizontal_Metal = nullptr;
    MTL::Texture* m_pRenderTexture_Blur2_Vertical_Metal = nullptr;
    MTL::Texture* m_pRenderTexture_Final_Metal = nullptr;
#endif // __APPLE__

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

    void renderHorizontalBlurOpenGL(GLuint nFBO, GLuint nOutputTexture, GLuint nInputTexture, float fBlurRadius, float fBlurSigma);
    void renderVerticalBlurOpenGL(GLuint nFBO, GLuint nOutputTexture, GLuint nInputTexture, float fBlurRadius, float fBlurSigma);

#if __APPLE__
    void renderBlurMetal(Shader* pShader, MTL::Texture* pOutputTexture, MTL::Texture* pInputTexture, int nSize, float fBlurRadius, float fBlurSigma);
#endif // __APPLE__

    void renderComposite();
};

REGISTER_CLASS_NO_CREATOR(DifferenceOfGaussian);

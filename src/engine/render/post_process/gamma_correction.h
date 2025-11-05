#pragma once

#include "render_process_queue.h"

class Shader;


typedef unsigned int GLuint;

class GammaCorrection : public IRenderProcess
{
public:
    GammaCorrection(RenderProcessQueue* pQueue) : IRenderProcess(pQueue) {}

    void initialize() override;

    void renderProcess() override;

    inline float getGamma() const { return m_fGamma; }
    inline void setGamma(float fGamma) { m_fGamma = fGamma; }

private:
    GLuint m_nFBOID = 0;
    GLuint m_nRenderTexture = 0;

    int m_nRenderWidth, m_nRenderHeight;

    Shader* m_pShader = nullptr;
    const ShaderUniformHandle* m_pTextureUniform = nullptr;
    const ShaderUniformHandle* m_pGammaUniform = nullptr;

    float m_fGamma = 2.2f;

    void initializeShader();

    void render();
};

REGISTER_CLASS_NO_CREATOR(GammaCorrection);

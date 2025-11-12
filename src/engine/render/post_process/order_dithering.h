#pragma once

#include "render_process_queue.h"

class Shader;


typedef unsigned int GLuint;

class OrderDithering : public IRenderProcess
{
public:
    OrderDithering(RenderProcessQueue* pQueue) : IRenderProcess(pQueue) {}

    void initialize() override;

    void renderProcess() override;

    void onWindowResize() override;

private:
    GLuint m_nFBOID = 0;
    GLuint m_nRenderTexture = 0;

    int m_nRenderWidth, m_nRenderHeight;

    Shader* m_pShader = nullptr;
    const ShaderUniformHandle* m_pTextureUniform = nullptr;

    void initializeShader();

    void render();
};

REGISTER_CLASS_NO_CREATOR(OrderDithering);

#include "gamma_correction.h"

#include <glad/glad.h>
#include "../shader_loader.h"
#include "../core/renderer.h"
#include "../../core/debug_macro.h"
#include "../../core/window.h"


void GammaCorrection::initialize()
{
    m_nRenderWidth = m_pProcessQueue->getRenderWidth();
    m_nRenderHeight = m_pProcessQueue->getRenderHeight();

    if (Window::ins->isUsingOpenGL())
    {
        initializeRenderTextureAndFBO(m_nFBOID, m_nRenderTexture, m_nRenderWidth, m_nRenderHeight);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        initializeRenderTextureAndFBO(m_pMetalRenderTexture, m_nRenderWidth, m_nRenderHeight);
    }
#endif // __APPLE__

    initializeShader();
}

void GammaCorrection::initializeShader()
{
    m_pShader = ShaderLoader::getInstance()->getShader("gamma_correction");

    if (Window::ins->isUsingOpenGL())
    {
        m_pGammaUniform = m_pShader->getUniformHandle("u_gamma");
        m_pTextureUniform = m_pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);

        glBindBuffer(GL_ARRAY_BUFFER, m_pProcessQueue->getFullScreenVertexBuffer());
        glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());

        registerShaderPosAndUV(m_pShader);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void GammaCorrection::renderProcess()
{
    render();
}

void GammaCorrection::render()
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    if (Window::ins->isUsingOpenGL())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID);
        glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(m_pShader->getProgram());

        glUniform1i(m_pTextureUniform->m_nLocation, 0); // Texture unit 0

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_pProcessQueue->getFinalRenderTexture());

        glUniform1f(m_pGammaUniform->m_nLocation, m_fGamma);

        glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
        INCREASE_DRAW_CALL_COUNT(2);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        glBindVertexArray(0); // Unbind the vertex array
        glUseProgram(0);

        m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        Window::ins->setCurrentDrawingTexture(m_pMetalRenderTexture);

        MTL::RenderCommandEncoder* pEncoder = Window::ins->getCurrentFrameRenderEncoder();

        RenderProcessQueue* pQueue = Window::ins->getRenderProcessQueue();
        pEncoder->setRenderPipelineState(m_pShader->getMetalPipelineState());
        pEncoder->setVertexBuffer(pQueue->getMetalFullScreenVertexBuffer(), 0, 0);
        pEncoder->setFragmentBytes(&m_fGamma, sizeof(float), 1);
        pEncoder->setFragmentTexture(pQueue->getFinalMetalRenderTexture(), 0);
        pEncoder->setFragmentSamplerState(MetalRenderer::m_pLinearSampler, 0);

        pEncoder->drawPrimitives(MTL::PrimitiveTypeTriangleStrip, NS::UInteger(0), NS::UInteger(4));
        INCREASE_DRAW_CALL_COUNT(2);

        pQueue->setFinalMetalRenderTexture(m_pMetalRenderTexture);
    }
#endif // __APPLE__
}

void GammaCorrection::onWindowResize()
{
    m_nRenderWidth = m_pProcessQueue->getRenderWidth();
    m_nRenderHeight = m_pProcessQueue->getRenderHeight();

    if (Window::ins->isUsingOpenGL())
    {
        initializeRenderTextureAndFBO(m_nFBOID, m_nRenderTexture, m_nRenderWidth, m_nRenderHeight, false);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        initializeRenderTextureAndFBO(m_pMetalRenderTexture, m_nRenderWidth, m_nRenderHeight);
    }
#endif // __APPLE__
}

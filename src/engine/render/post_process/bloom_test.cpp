#include "bloom_test.h"

#include <glad/gl.h>
#include "../vertex.h"
#include "../shader.h"
#include "../shader_loader.h"
#include "../core/renderer.h"
#include "../../core/window.h"
#include "../../core/debug_macro.h"

#define BLUR_TEXTURE_RATIO .25f

void BloomTest::initialize()
{
    m_nRenderWidth = m_pProcessQueue->getRenderWidth();
    m_nRenderHeight = m_pProcessQueue->getRenderHeight();

    int nOneForthWidth = static_cast<int>(m_nRenderWidth * BLUR_TEXTURE_RATIO);
    int nOneForthHeight = static_cast<int>(m_nRenderHeight * BLUR_TEXTURE_RATIO);

    if (Window::ins->isUsingOpenGL())
    {
        initializeRenderTextureAndFBO(m_nFBOID_ColorHighlight, m_nRenderTexture_ColorHighlight, m_nRenderWidth, m_nRenderHeight);
        initializeRenderTextureAndFBO(m_nFBOID_HorizontalBlur, m_nRenderTexture_HorizontalBlur, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_VerticalBlur, m_nRenderTexture_VerticalBlur, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_Final, m_nRenderTexture_Final, m_nRenderWidth, m_nRenderHeight);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the FBO when done
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind any texture when done
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        initializeRenderTextureAndFBO(m_pMetalRenderTexture_ColorHighlight, m_nRenderWidth, m_nRenderHeight);
        initializeRenderTextureAndFBO(m_pMetalRenderTexture_HorizontalBlur, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pMetalRenderTexture_VerticalBlur, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pMetalFinalRenderTexture, m_nRenderWidth, m_nRenderHeight);
    }
#endif // __APPLE__

    initializeQuad();
}

void BloomTest::initializeQuad()
{
    m_pColorHighlightShader = ShaderLoader::getInstance()->getShader("bloom_filter");
    m_pHorizontalBlurShader = ShaderLoader::getInstance()->getShader("horizontal_blur");
    m_pVerticalBlurShader = ShaderLoader::getInstance()->getShader("vertical_blur");
    m_pCompositeShader = ShaderLoader::getInstance()->getShader("bloom_composite");
    
    if (Window::ins->isUsingOpenGL())
    {
        m_pTextureUniform_ColorHighlight = m_pColorHighlightShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
        m_pThresholdUniform_ColorHighlight = m_pColorHighlightShader->getUniformHandle("u_threshold");

        m_pTextureUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
        m_pTextureWidthUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle("u_textureWidth");
        m_pBlurRadiusUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle("u_blurRadius");
        m_pBlurSigmaUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle("u_blurSigma");

        m_pTextureUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
        m_pTextureHeightUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle("u_textureHeight");
        m_pBlurRadiusUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle("u_blurRadius");
        m_pBlurSigmaUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle("u_blurSigma");

        m_pOriginalTextureUniform = m_pCompositeShader->getUniformHandle("u_originalScreenTexture");
        m_pBloomTextureUniform = m_pCompositeShader->getUniformHandle("u_bloomTexture");
        m_pBloomTextureScaleUniform = m_pCompositeShader->getUniformHandle("u_bloomTextureScale");
        m_pIntensityUniform = m_pCompositeShader->getUniformHandle("u_intensity");

        glBindBuffer(GL_ARRAY_BUFFER, m_pProcessQueue->getFullScreenVertexBuffer());
        glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());

        registerShaderPosAndUV(m_pColorHighlightShader);
        registerShaderPosAndUV(m_pHorizontalBlurShader);
        registerShaderPosAndUV(m_pVerticalBlurShader);
        registerShaderPosAndUV(m_pCompositeShader);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void BloomTest::renderProcess()
{
    if (m_nBloomProcessDebugStep <= 0)
        return;

    m_nOriginalRenderTexture = m_pProcessQueue->getFinalRenderTexture();
#if __APPLE__
    m_pMetalOriginalRenderTexture = m_pProcessQueue->getFinalMetalRenderTexture();
#endif // __APPLE__

    renderColorHighlight();

    if (m_nBloomProcessDebugStep == 1)
        return;

    renderHorizontalBlur();

    if (m_nBloomProcessDebugStep == 2)
        return;

    renderVerticalBlur();

    if (m_nBloomProcessDebugStep == 3)
        return;

    renderComposite();
}

void BloomTest::renderColorHighlight()
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pColorHighlightShader, "Shader must be set before drawing the quad");

    if (Window::ins->isUsingOpenGL())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_ColorHighlight);
        glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(m_pColorHighlightShader->getProgram());

        glUniform1i(m_pTextureUniform_ColorHighlight->m_nLocation, 0); // Texture unit 0
        glUniform1f(m_pThresholdUniform_ColorHighlight->m_nLocation, m_fHighlightThreshold);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_nOriginalRenderTexture);

        glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
        INCREASE_DRAW_CALL_COUNT(2);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        glBindVertexArray(0); // Unbind the vertex array
        glUseProgram(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_ColorHighlight);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        Window::ins->setCurrentDrawingTexture(m_pMetalRenderTexture_ColorHighlight);

        MTL::RenderCommandEncoder* pEncoder = Window::ins->getCurrentFrameRenderEncoder();

        RenderProcessQueue* pQueue = Window::ins->getRenderProcessQueue();
        pEncoder->setRenderPipelineState(m_pColorHighlightShader->getMetalPipelineState());
        pEncoder->setVertexBuffer(pQueue->getMetalFullScreenVertexBuffer(), 0, 0);
        pEncoder->setFragmentBytes(&m_fHighlightThreshold, sizeof(float), 1);
        pEncoder->setFragmentTexture(pQueue->getFinalMetalRenderTexture(), 0);
        pEncoder->setFragmentSamplerState(MetalRenderer::m_pLinearSampler, 0);

        pEncoder->drawPrimitives(MTL::PrimitiveTypeTriangleStrip, NS::UInteger(0), NS::UInteger(4));
        INCREASE_DRAW_CALL_COUNT(2);

        pQueue->setFinalMetalRenderTexture(m_pMetalRenderTexture_ColorHighlight);
    }
#endif // __APPLE__
}

void BloomTest::renderHorizontalBlur()
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pHorizontalBlurShader, "Shader must be set before drawing the quad");

    if (Window::ins->isUsingOpenGL())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_HorizontalBlur);
        glViewport(0, 0, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(m_pHorizontalBlurShader->getProgram());

        glUniform1i(m_pTextureUniform_HorizontalBlur->m_nLocation, 0); // Texture unit 0
        glUniform1i(m_pTextureWidthUniform_HorizontalBlur->m_nLocation, m_nRenderWidth * BLUR_TEXTURE_RATIO);
        glUniform1f(m_pBlurRadiusUniform_HorizontalBlur->m_nLocation, m_fBlurRadius);
        glUniform1f(m_pBlurSigmaUniform_HorizontalBlur->m_nLocation, m_fBlurSigma);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_ColorHighlight);

        glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
        INCREASE_DRAW_CALL_COUNT(2);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        glBindVertexArray(0); // Unbind the vertex array
        glUseProgram(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_HorizontalBlur);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        Window::ins->setCurrentDrawingTexture(m_pMetalRenderTexture_HorizontalBlur);

        MTL::RenderCommandEncoder* pEncoder = Window::ins->getCurrentFrameRenderEncoder();

        RenderProcessQueue* pQueue = Window::ins->getRenderProcessQueue();
        pEncoder->setRenderPipelineState(m_pHorizontalBlurShader->getMetalPipelineState());
        pEncoder->setVertexBuffer(pQueue->getMetalFullScreenVertexBuffer(), 0, 0);

        struct {
            int nSize;
            int nRadius;
            float fSigma;
        } sizeRadiusSigma;
        sizeRadiusSigma.nSize = m_nRenderWidth * BLUR_TEXTURE_RATIO;
        sizeRadiusSigma.nRadius = m_fBlurRadius;
        sizeRadiusSigma.fSigma = m_fBlurSigma;
        pEncoder->setFragmentBytes(&sizeRadiusSigma, sizeof(sizeRadiusSigma), 1);

        pEncoder->setFragmentTexture(m_pMetalRenderTexture_ColorHighlight, 0);
        pEncoder->setFragmentSamplerState(MetalRenderer::m_pLinearSampler, 0);

        pEncoder->drawPrimitives(MTL::PrimitiveTypeTriangleStrip, NS::UInteger(0), NS::UInteger(4));
        INCREASE_DRAW_CALL_COUNT(2);

        pQueue->setFinalMetalRenderTexture(m_pMetalRenderTexture_HorizontalBlur);
    }
#endif // __APPLE__
}

void BloomTest::renderVerticalBlur()
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pVerticalBlurShader, "Shader must be set before drawing the quad");

    if (Window::ins->isUsingOpenGL())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_VerticalBlur);
        glViewport(0, 0, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(m_pVerticalBlurShader->getProgram());

        glUniform1i(m_pTextureUniform_VerticalBlur->m_nLocation, 0); // Texture unit 0
        glUniform1i(m_pTextureHeightUniform_VerticalBlur->m_nLocation, m_nRenderHeight * BLUR_TEXTURE_RATIO);
        glUniform1f(m_pBlurRadiusUniform_VerticalBlur->m_nLocation, m_fBlurRadius);
        glUniform1f(m_pBlurSigmaUniform_VerticalBlur->m_nLocation, m_fBlurSigma);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_HorizontalBlur);

        glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
        INCREASE_DRAW_CALL_COUNT(2);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        glBindVertexArray(0); // Unbind the vertex array
        glUseProgram(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_VerticalBlur);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        Window::ins->setCurrentDrawingTexture(m_pMetalRenderTexture_VerticalBlur);

        MTL::RenderCommandEncoder* pEncoder = Window::ins->getCurrentFrameRenderEncoder();

        RenderProcessQueue* pQueue = Window::ins->getRenderProcessQueue();
        pEncoder->setRenderPipelineState(m_pVerticalBlurShader->getMetalPipelineState());
        pEncoder->setVertexBuffer(pQueue->getMetalFullScreenVertexBuffer(), 0, 0);

        struct {
            int nSize;
            int nRadius;
            float fSigma;
        } sizeRadiusSigma;
        sizeRadiusSigma.nSize = m_nRenderHeight * BLUR_TEXTURE_RATIO;
        sizeRadiusSigma.nRadius = m_fBlurRadius;
        sizeRadiusSigma.fSigma = m_fBlurSigma;
        pEncoder->setFragmentBytes(&sizeRadiusSigma, sizeof(sizeRadiusSigma), 1);

        pEncoder->setFragmentTexture(m_pMetalRenderTexture_HorizontalBlur, 0);
        pEncoder->setFragmentSamplerState(MetalRenderer::m_pLinearSampler, 0);

        pEncoder->drawPrimitives(MTL::PrimitiveTypeTriangleStrip, NS::UInteger(0), NS::UInteger(4));
        INCREASE_DRAW_CALL_COUNT(2);

        pQueue->setFinalMetalRenderTexture(m_pMetalRenderTexture_VerticalBlur);
    }
#endif // __APPLE__
}

void BloomTest::renderComposite()
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pCompositeShader, "Shader must be set before drawing the quad");

    if (Window::ins->isUsingOpenGL())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_Final);
        glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(m_pCompositeShader->getProgram());

        glUniform1i(m_pOriginalTextureUniform->m_nLocation, 0);
        glUniform1i(m_pBloomTextureUniform->m_nLocation, 1);
        glUniform1f(m_pBloomTextureScaleUniform->m_nLocation, 1);
        glUniform1f(m_pIntensityUniform->m_nLocation, m_nIntensity);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_nOriginalRenderTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_VerticalBlur);

        glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
        INCREASE_DRAW_CALL_COUNT(2);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        glBindVertexArray(0); // Unbind the vertex array
        glUseProgram(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_Final);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        Window::ins->setCurrentDrawingTexture(m_pMetalFinalRenderTexture);

        MTL::RenderCommandEncoder* pEncoder = Window::ins->getCurrentFrameRenderEncoder();

        RenderProcessQueue* pQueue = Window::ins->getRenderProcessQueue();
        pEncoder->setRenderPipelineState(m_pCompositeShader->getMetalPipelineState());
        pEncoder->setVertexBuffer(pQueue->getMetalFullScreenVertexBuffer(), 0, 0);

        struct {
            float fBloomTextureScale;
            float fIntensity;
        } bloomParams;
        bloomParams.fBloomTextureScale = 1.0f;
        bloomParams.fIntensity = m_nIntensity;
        pEncoder->setFragmentBytes(&bloomParams, sizeof(bloomParams), 1);

        pEncoder->setFragmentTexture(m_pMetalOriginalRenderTexture, 0);
        pEncoder->setFragmentTexture(m_pMetalRenderTexture_VerticalBlur, 1);
        pEncoder->setFragmentSamplerState(MetalRenderer::m_pLinearSampler, 0);

        pEncoder->drawPrimitives(MTL::PrimitiveTypeTriangleStrip, NS::UInteger(0), NS::UInteger(4));
        INCREASE_DRAW_CALL_COUNT(2);

        pQueue->setFinalMetalRenderTexture(m_pMetalFinalRenderTexture);
    }
#endif // __APPLE__
}

void BloomTest::onWindowResize()
{
    m_nRenderWidth = m_pProcessQueue->getRenderWidth();
    m_nRenderHeight = m_pProcessQueue->getRenderHeight();

    int nOneForthWidth = static_cast<int>(m_nRenderWidth * BLUR_TEXTURE_RATIO);
    int nOneForthHeight = static_cast<int>(m_nRenderHeight * BLUR_TEXTURE_RATIO);

    if (Window::ins->isUsingOpenGL())
    {
        initializeRenderTextureAndFBO(m_nFBOID_ColorHighlight, m_nRenderTexture_ColorHighlight, m_nRenderWidth, m_nRenderHeight);
        initializeRenderTextureAndFBO(m_nFBOID_HorizontalBlur, m_nRenderTexture_HorizontalBlur, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_VerticalBlur, m_nRenderTexture_VerticalBlur, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_Final, m_nRenderTexture_Final, m_nRenderWidth, m_nRenderHeight);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the FBO when done
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind any texture when done
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        initializeRenderTextureAndFBO(m_pMetalRenderTexture_ColorHighlight, m_nRenderWidth, m_nRenderHeight);
        initializeRenderTextureAndFBO(m_pMetalRenderTexture_HorizontalBlur, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pMetalRenderTexture_VerticalBlur, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pMetalFinalRenderTexture, m_nRenderWidth, m_nRenderHeight);
    }
#endif // __APPLE__
}

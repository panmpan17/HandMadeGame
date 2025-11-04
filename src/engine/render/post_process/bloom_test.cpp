#include "bloom_test.h"

#include <glad/gl.h>
#include "../window.h"
#include "../debug_macro.h"
#include "../draw/vertex.h"
#include "../draw/shader.h"
#include "../draw/shader_loader.h"

#define BLUR_TEXTURE_RATIO .25f

void BloomTest::initialize()
{
    m_nRenderWidth = m_pProcessQueue->getRenderWidth();
    m_nRenderHeight = m_pProcessQueue->getRenderHeight();

    int nOneForthWidth = static_cast<int>(m_nRenderWidth * BLUR_TEXTURE_RATIO);
    int nOneForthHeight = static_cast<int>(m_nRenderHeight * BLUR_TEXTURE_RATIO);
    initializeRenderTextureAndFBO(m_nFBOID_ColorHighlight, m_nRenderTexture_ColorHighlight, m_nRenderWidth, m_nRenderHeight);
    initializeRenderTextureAndFBO(m_nFBOID_HorizontalBlur, m_nRenderTexture_HorizontalBlur, nOneForthWidth, nOneForthHeight);
    initializeRenderTextureAndFBO(m_nFBOID_VerticalBlur, m_nRenderTexture_VerticalBlur, nOneForthWidth, nOneForthHeight);
    initializeRenderTextureAndFBO(m_nFBOID_Final, m_nRenderTexture_Final, m_nRenderWidth, m_nRenderHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the FBO when done
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind any texture when done

    initializeQuad();
}

void BloomTest::initializeQuad()
{
    m_pColorHighlightShader = ShaderLoader::getInstance()->getShader("bloom_filter");
    m_pTextureUniform_ColorHighlight = m_pColorHighlightShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
    m_pThresholdUniform_ColorHighlight = m_pColorHighlightShader->getUniformHandle("u_threshold");

    m_pHorizontalBlurShader = ShaderLoader::getInstance()->getShader("horizontal_blur");
    m_pTextureUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
    m_pTextureWidthUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle("u_textureWidth");
    m_pBlurRadiusUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle("u_blurRadius");
    m_pBlurSigmaUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle("u_blurSigma");

    m_pVerticalBlurShader = ShaderLoader::getInstance()->getShader("vertical_blur");
    m_pTextureUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
    m_pTextureHeightUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle("u_textureHeight");
    m_pBlurRadiusUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle("u_blurRadius");
    m_pBlurSigmaUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle("u_blurSigma");

    m_pCompositeShader = ShaderLoader::getInstance()->getShader("bloom_composite");
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

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BloomTest::renderProcess()
{
    if (m_nBloomProcessDebugStep <= 0)
        return;

    m_nOriginalRenderTexture = m_pProcessQueue->getFinalRenderTexture();

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
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_ColorHighlight);
}

void BloomTest::renderHorizontalBlur()
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pHorizontalBlurShader, "Shader must be set before drawing the quad");

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
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_HorizontalBlur);
}

void BloomTest::renderVerticalBlur()
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pVerticalBlurShader, "Shader must be set before drawing the quad");

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
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_VerticalBlur);
}

void BloomTest::renderComposite()
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pCompositeShader, "Shader must be set before drawing the quad");

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
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_Final);
}

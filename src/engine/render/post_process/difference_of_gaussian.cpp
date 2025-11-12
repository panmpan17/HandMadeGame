#include "difference_of_gaussian.h"

#include <glad/gl.h>
#include "../vertex.h"
#include "../shader_loader.h"
#include "../../core/window.h"
#include "../../core/debug_macro.h"


#define BLUR_TEXTURE_RATIO .25f


void DifferenceOfGaussian::initialize()
{
    m_nRenderWidth = m_pProcessQueue->getRenderWidth();
    m_nRenderHeight = m_pProcessQueue->getRenderHeight();

    int nOneForthWidth = static_cast<int>(m_nRenderWidth * BLUR_TEXTURE_RATIO);
    int nOneForthHeight = static_cast<int>(m_nRenderHeight * BLUR_TEXTURE_RATIO);
    initializeRenderTextureAndFBO(m_nFBOID_Blur1_Horizontal, m_nRenderTexture_Blur1_Horizontal, nOneForthWidth, nOneForthHeight);
    initializeRenderTextureAndFBO(m_nFBOID_BLur1_Vertical, m_nRenderTexture_Blur1_Vertical, nOneForthWidth, nOneForthHeight);
    initializeRenderTextureAndFBO(m_nFBOID_Blur2_Horizontal, m_nRenderTexture_Blur2_Horizontal, nOneForthWidth, nOneForthHeight);
    initializeRenderTextureAndFBO(m_nFBOID_BLur2_Vertical, m_nRenderTexture_Blur2_Vertical, nOneForthWidth, nOneForthHeight);
    initializeRenderTextureAndFBO(m_nFBOID_Final, m_nRenderTexture_Final, m_nRenderWidth, m_nRenderHeight);

    initializeQuad();
}

void DifferenceOfGaussian::initializeQuad()
{
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

    m_pCompositeShader = ShaderLoader::getInstance()->getShader("difference_of_gaussian_composite");
    m_pOriginalTextureUniform = m_pCompositeShader->getUniformHandle("u_originalTexture");
    m_pBlur1TextureUniform = m_pCompositeShader->getUniformHandle("u_blur1Texture");
    m_pBlur2TextureUniform = m_pCompositeShader->getUniformHandle("u_blur2Texture");

    glBindBuffer(GL_ARRAY_BUFFER, m_pProcessQueue->getFullScreenVertexBuffer());
    glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());

    registerShaderPosAndUV(m_pHorizontalBlurShader);
    registerShaderPosAndUV(m_pVerticalBlurShader);
    registerShaderPosAndUV(m_pCompositeShader);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void DifferenceOfGaussian::renderProcess()
{
    m_nOriginalRenderTexture = m_pProcessQueue->getFinalRenderTexture();

    renderBlur1Horizontal();
    renderBlur1Vertical();

    renderBlur2Horizontal();
    renderBlur2Vertical();

    renderComposite();
}

void DifferenceOfGaussian::renderBlur1Horizontal()
{
    ASSERT(m_pHorizontalBlurShader, "Shader must be set before drawing the quad");

    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_Blur1_Horizontal);
    glViewport(0, 0, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pHorizontalBlurShader->getProgram());

    glUniform1i(m_pTextureUniform_HorizontalBlur->m_nLocation, 0); // Texture unit 0
    glUniform1i(m_pTextureWidthUniform_HorizontalBlur->m_nLocation, m_nRenderWidth * BLUR_TEXTURE_RATIO);
    glUniform1f(m_pBlurRadiusUniform_HorizontalBlur->m_nLocation, m_fBlurRadius1);
    glUniform1f(m_pBlurSigmaUniform_HorizontalBlur->m_nLocation, m_fBlurSigma1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nOriginalRenderTexture);

    glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_Blur1_Horizontal);
}

void DifferenceOfGaussian::renderBlur1Vertical()
{
    ASSERT(m_pVerticalBlurShader, "Shader must be set before drawing the quad");

    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_BLur1_Vertical);
    glViewport(0, 0, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pVerticalBlurShader->getProgram());

    glUniform1i(m_pTextureUniform_VerticalBlur->m_nLocation, 0); // Texture unit 0
    glUniform1i(m_pTextureHeightUniform_VerticalBlur->m_nLocation, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glUniform1f(m_pBlurRadiusUniform_VerticalBlur->m_nLocation, m_fBlurRadius1);
    glUniform1f(m_pBlurSigmaUniform_VerticalBlur->m_nLocation, m_fBlurSigma1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_Blur1_Horizontal);

    glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_Blur1_Vertical);
}

void DifferenceOfGaussian::renderBlur2Horizontal()
{
    ASSERT(m_pHorizontalBlurShader, "Shader must be set before drawing the quad");

    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_Blur2_Horizontal);
    glViewport(0, 0, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pHorizontalBlurShader->getProgram());

    glUniform1i(m_pTextureUniform_HorizontalBlur->m_nLocation, 0); // Texture unit 0
    glUniform1i(m_pTextureWidthUniform_HorizontalBlur->m_nLocation, m_nRenderWidth * BLUR_TEXTURE_RATIO);
    glUniform1f(m_pBlurRadiusUniform_HorizontalBlur->m_nLocation, m_fBlurRadius2);
    glUniform1f(m_pBlurSigmaUniform_HorizontalBlur->m_nLocation, m_fBlurSigma2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nOriginalRenderTexture);

    glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the
    INCREASE_DRAW_CALL_COUNT();
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_Blur2_Horizontal);
}

void DifferenceOfGaussian::renderBlur2Vertical()
{
    ASSERT(m_pVerticalBlurShader, "Shader must be set before drawing the quad");

    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_BLur2_Vertical);
    glViewport(0, 0, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pVerticalBlurShader->getProgram());

    glUniform1i(m_pTextureUniform_VerticalBlur->m_nLocation, 0); // Texture unit 0
    glUniform1i(m_pTextureHeightUniform_VerticalBlur->m_nLocation, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glUniform1f(m_pBlurRadiusUniform_VerticalBlur->m_nLocation, m_fBlurRadius2);
    glUniform1f(m_pBlurSigmaUniform_VerticalBlur->m_nLocation, m_fBlurSigma2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_Blur2_Horizontal);

    glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_Blur2_Vertical);
}

void DifferenceOfGaussian::renderComposite()
{
    ASSERT(m_pCompositeShader, "Shader must be set before drawing the quad");

    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_Final);
    glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pCompositeShader->getProgram());

    glUniform1i(m_pOriginalTextureUniform->m_nLocation, 0);
    glUniform1i(m_pBlur1TextureUniform->m_nLocation, 1);
    glUniform1i(m_pBlur2TextureUniform->m_nLocation, 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nOriginalRenderTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_Blur1_Vertical);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_Blur2_Vertical);

    glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_Final);
}

void DifferenceOfGaussian::onWindowResize()
{
    m_nRenderWidth = m_pProcessQueue->getRenderWidth();
    m_nRenderHeight = m_pProcessQueue->getRenderHeight();

    glDeleteTextures(1, &m_nRenderTexture_Blur1_Horizontal);
    glDeleteTextures(1, &m_nRenderTexture_Blur1_Vertical);
    glDeleteTextures(1, &m_nRenderTexture_Blur2_Horizontal);
    glDeleteTextures(1, &m_nRenderTexture_Blur2_Vertical);
    glDeleteTextures(1, &m_nRenderTexture_Final);

    int nOneForthWidth = static_cast<int>(m_nRenderWidth * BLUR_TEXTURE_RATIO);
    int nOneForthHeight = static_cast<int>(m_nRenderHeight * BLUR_TEXTURE_RATIO);
    initializeRenderTextureAndFBO(m_nFBOID_Blur1_Horizontal, m_nRenderTexture_Blur1_Horizontal, nOneForthWidth, nOneForthHeight);
    initializeRenderTextureAndFBO(m_nFBOID_BLur1_Vertical, m_nRenderTexture_Blur1_Vertical, nOneForthWidth, nOneForthHeight);
    initializeRenderTextureAndFBO(m_nFBOID_Blur2_Horizontal, m_nRenderTexture_Blur2_Horizontal, nOneForthWidth, nOneForthHeight);
    initializeRenderTextureAndFBO(m_nFBOID_BLur2_Vertical, m_nRenderTexture_Blur2_Vertical, nOneForthWidth, nOneForthHeight);
    initializeRenderTextureAndFBO(m_nFBOID_Final, m_nRenderTexture_Final, m_nRenderWidth, m_nRenderHeight);
}


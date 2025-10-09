#include "difference_of_gaussian.h"

#include <glad/gl.h>
#include "../window.h"
#include "../debug_macro.h"
#include "../draw/vertex.h"
#include "../draw/shader.h"
#include "../draw/shader_loader.h"


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
    m_nTextureUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformLocation("u_tex0");
    m_nTextureWidthUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformLocation("u_textureWidth");
    m_nBlurRadiusUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformLocation("u_blurRadius");
    m_nBlurSigmaUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformLocation("u_blurSigma");

    m_pVerticalBlurShader = ShaderLoader::getInstance()->getShader("vertical_blur");
    m_nTextureUniform_VerticalBlur = m_pVerticalBlurShader->getUniformLocation("u_tex0");
    m_nTextureHeightUniform_VerticalBlur = m_pVerticalBlurShader->getUniformLocation("u_textureHeight");
    m_nBlurRadiusUniform_VerticalBlur = m_pVerticalBlurShader->getUniformLocation("u_blurRadius");
    m_nBlurSigmaUniform_VerticalBlur = m_pVerticalBlurShader->getUniformLocation("u_blurSigma");

    m_pCompositeShader = ShaderLoader::getInstance()->getShader("difference_of_gaussian_composite");
    m_nOriginalTextureUniform = m_pCompositeShader->getUniformLocation("u_originalTexture");
    m_nBlur1TextureUniform = m_pCompositeShader->getUniformLocation("u_blur1Texture");
    m_nBlur2TextureUniform = m_pCompositeShader->getUniformLocation("u_blur2Texture");

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

    glUniform1i(m_nTextureUniform_HorizontalBlur, 0); // Texture unit 0
    glUniform1i(m_nTextureWidthUniform_HorizontalBlur, m_nRenderWidth * BLUR_TEXTURE_RATIO);
    glUniform1f(m_nBlurRadiusUniform_HorizontalBlur, m_fBlurRadius1);
    glUniform1f(m_nBlurSigmaUniform_HorizontalBlur, m_fBlurSigma1);

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

    glUniform1i(m_nTextureUniform_VerticalBlur, 0); // Texture unit 0
    glUniform1i(m_nTextureHeightUniform_VerticalBlur, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glUniform1f(m_nBlurRadiusUniform_VerticalBlur, m_fBlurRadius1);
    glUniform1f(m_nBlurSigmaUniform_VerticalBlur, m_fBlurSigma1);

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

    glUniform1i(m_nTextureUniform_HorizontalBlur, 0); // Texture unit 0
    glUniform1i(m_nTextureWidthUniform_HorizontalBlur, m_nRenderWidth * BLUR_TEXTURE_RATIO);
    glUniform1f(m_nBlurRadiusUniform_HorizontalBlur, m_fBlurRadius2);
    glUniform1f(m_nBlurSigmaUniform_HorizontalBlur, m_fBlurSigma2);

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

    glUniform1i(m_nTextureUniform_VerticalBlur, 0); // Texture unit 0
    glUniform1i(m_nTextureHeightUniform_VerticalBlur, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glUniform1f(m_nBlurRadiusUniform_VerticalBlur, m_fBlurRadius2);
    glUniform1f(m_nBlurSigmaUniform_VerticalBlur, m_fBlurSigma2);

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

    glUniform1i(m_nOriginalTextureUniform, 0);
    glUniform1i(m_nBlur1TextureUniform, 1);
    glUniform1i(m_nBlur2TextureUniform, 2);

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


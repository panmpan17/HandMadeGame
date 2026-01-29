#include "difference_of_gaussian.h"

#include <glad/gl.h>
#include "../vertex.h"
#include "../shader_loader.h"
#include "../core/renderer.h"
#include "../../core/window.h"
#include "../../core/debug_macro.h"


#define BLUR_TEXTURE_RATIO .25f


void DifferenceOfGaussian::initialize()
{
    m_nRenderWidth = m_pProcessQueue->getRenderWidth();
    m_nRenderHeight = m_pProcessQueue->getRenderHeight();

    int nOneForthWidth = static_cast<int>(m_nRenderWidth * BLUR_TEXTURE_RATIO);
    int nOneForthHeight = static_cast<int>(m_nRenderHeight * BLUR_TEXTURE_RATIO);

    if (Window::ins->isUsingOpenGL())
    {
        initializeRenderTextureAndFBO(m_nFBOID_Blur1_Horizontal, m_nRenderTexture_Blur1_Horizontal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_BLur1_Vertical, m_nRenderTexture_Blur1_Vertical, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_Blur2_Horizontal, m_nRenderTexture_Blur2_Horizontal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_BLur2_Vertical, m_nRenderTexture_Blur2_Vertical, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_Final, m_nRenderTexture_Final, m_nRenderWidth, m_nRenderHeight);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        initializeRenderTextureAndFBO(m_pOriginalRenderTexture_Metal, m_nRenderWidth, m_nRenderHeight);
        initializeRenderTextureAndFBO(m_pRenderTexture_Blur1_Horizontal_Metal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pRenderTexture_Blur1_Vertical_Metal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pRenderTexture_Blur2_Horizontal_Metal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pRenderTexture_Blur2_Vertical_Metal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pRenderTexture_Final_Metal, m_nRenderWidth, m_nRenderHeight);
    }
#endif

    initializeQuad();
}

void DifferenceOfGaussian::initializeQuad()
{
    m_pHorizontalBlurShader = ShaderLoader::getInstance()->getShader("horizontal_blur");
    m_pVerticalBlurShader = ShaderLoader::getInstance()->getShader("vertical_blur");
    m_pCompositeShader = ShaderLoader::getInstance()->getShader("difference_of_gaussian_composite");

    if (Window::ins->isUsingOpenGL())
    {
        m_pTextureUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
        m_pTextureWidthUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle("u_textureWidth");
        m_pBlurRadiusUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle("u_blurRadius");
        m_pBlurSigmaUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformHandle("u_blurSigma");

        m_pTextureUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
        m_pTextureHeightUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle("u_textureHeight");
        m_pBlurRadiusUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle("u_blurRadius");
        m_pBlurSigmaUniform_VerticalBlur = m_pVerticalBlurShader->getUniformHandle("u_blurSigma");

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
}


void DifferenceOfGaussian::renderProcess()
{
    if (Window::ins->isUsingOpenGL())
    {
        m_nOriginalRenderTexture = m_pProcessQueue->getFinalRenderTexture();

        // The first blur pass
        renderHorizontalBlurOpenGL(m_nFBOID_Blur1_Horizontal, m_nRenderTexture_Blur1_Horizontal, m_nOriginalRenderTexture, m_fBlurRadius1, m_fBlurSigma1);
        renderVerticalBlurOpenGL(m_nFBOID_BLur1_Vertical, m_nRenderTexture_Blur1_Vertical, m_nRenderTexture_Blur1_Horizontal, m_fBlurRadius1, m_fBlurSigma1);

        // The second blur pass
        renderHorizontalBlurOpenGL(m_nFBOID_Blur2_Horizontal, m_nRenderTexture_Blur2_Horizontal, m_nOriginalRenderTexture, m_fBlurRadius2, m_fBlurSigma2);
        renderVerticalBlurOpenGL(m_nFBOID_BLur2_Vertical, m_nRenderTexture_Blur2_Vertical, m_nRenderTexture_Blur2_Horizontal, m_fBlurRadius2, m_fBlurSigma2);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        m_pOriginalRenderTexture_Metal = m_pProcessQueue->getFinalMetalRenderTexture();

        // The first blur pass
        renderBlurMetal(m_pHorizontalBlurShader, m_pRenderTexture_Blur1_Horizontal_Metal, m_pOriginalRenderTexture_Metal, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_fBlurRadius1, m_fBlurSigma1);
        renderBlurMetal(m_pVerticalBlurShader, m_pRenderTexture_Blur1_Vertical_Metal, m_pRenderTexture_Blur1_Horizontal_Metal, m_nRenderHeight * BLUR_TEXTURE_RATIO, m_fBlurRadius1, m_fBlurSigma1);

        // The second blur pass
        renderBlurMetal(m_pHorizontalBlurShader, m_pRenderTexture_Blur2_Horizontal_Metal, m_pOriginalRenderTexture_Metal, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_fBlurRadius2, m_fBlurSigma2);
        renderBlurMetal(m_pVerticalBlurShader, m_pRenderTexture_Blur2_Vertical_Metal, m_pRenderTexture_Blur2_Horizontal_Metal, m_nRenderHeight * BLUR_TEXTURE_RATIO, m_fBlurRadius2, m_fBlurSigma2);
    }
#endif // __APPLE__

    renderComposite();
}

void DifferenceOfGaussian::renderHorizontalBlurOpenGL(GLuint nFBO, GLuint nOutputTexture, GLuint nInputTexture, float fBlurRadius, float fBlurSigma)
{
    ASSERT(m_pHorizontalBlurShader, "Shader must be set before drawing the quad");

    glBindFramebuffer(GL_FRAMEBUFFER, nFBO);
    glViewport(0, 0, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pHorizontalBlurShader->getProgram());

    glUniform1i(m_pTextureUniform_HorizontalBlur->m_nLocation, 0); // Texture unit 0
    glUniform1i(m_pTextureWidthUniform_HorizontalBlur->m_nLocation, m_nRenderWidth * BLUR_TEXTURE_RATIO);
    glUniform1f(m_pBlurRadiusUniform_HorizontalBlur->m_nLocation, fBlurRadius);
    glUniform1f(m_pBlurSigmaUniform_HorizontalBlur->m_nLocation, fBlurSigma);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nInputTexture);

    glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the
    INCREASE_DRAW_CALL_COUNT(2);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(nOutputTexture);
}

void DifferenceOfGaussian::renderVerticalBlurOpenGL(GLuint nFBO, GLuint nOutputTexture, GLuint nInputTexture, float fBlurRadius, float fBlurSigma)
{
    ASSERT(m_pVerticalBlurShader, "Shader must be set before drawing the quad");

    glBindFramebuffer(GL_FRAMEBUFFER, nFBO);
    glViewport(0, 0, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pVerticalBlurShader->getProgram());

    glUniform1i(m_pTextureUniform_VerticalBlur->m_nLocation, 0); // Texture unit 0
    glUniform1i(m_pTextureHeightUniform_VerticalBlur->m_nLocation, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glUniform1f(m_pBlurRadiusUniform_VerticalBlur->m_nLocation, fBlurRadius);
    glUniform1f(m_pBlurSigmaUniform_VerticalBlur->m_nLocation, fBlurSigma);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nInputTexture);

    glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT(2);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(nOutputTexture);
}

void DifferenceOfGaussian::renderBlurMetal(Shader* pShader, MTL::Texture* pOutputTexture, MTL::Texture* pInputTexture, int nSize, float fBlurRadius, float fBlurSigma)
{
    Window::ins->setCurrentDrawingTexture(pOutputTexture);

    MTL::RenderCommandEncoder* pEncoder = Window::ins->getCurrentFrameRenderEncoder();

    pEncoder->setRenderPipelineState(pShader->getMetalPipelineState());
    pEncoder->setVertexBuffer(m_pProcessQueue->getMetalFullScreenVertexBuffer(), 0, 0);

    struct {
        int nSize;
        int nRadius;
        float fSigma;
    } sizeRadiusSigma;
    sizeRadiusSigma.nSize = nSize;//m_nRenderWidth * BLUR_TEXTURE_RATIO;
    sizeRadiusSigma.nRadius = fBlurRadius;
    sizeRadiusSigma.fSigma = fBlurSigma;
    pEncoder->setFragmentBytes(&sizeRadiusSigma, sizeof(sizeRadiusSigma), 1);

    pEncoder->setFragmentTexture(pInputTexture, 0);
    pEncoder->setFragmentSamplerState(MetalRenderer::m_pLinearSampler, 0);

    pEncoder->drawPrimitives(MTL::PrimitiveTypeTriangleStrip, NS::UInteger(0), NS::UInteger(4));
    INCREASE_DRAW_CALL_COUNT(2);
}

void DifferenceOfGaussian::renderComposite()
{
    ASSERT(m_pCompositeShader, "Shader must be set before drawing the quad");

    if (Window::ins->isUsingOpenGL())
    {
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
        INCREASE_DRAW_CALL_COUNT(2);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        glBindVertexArray(0); // Unbind the vertex array
        glUseProgram(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture_Final);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        Window::ins->setCurrentDrawingTexture(m_pRenderTexture_Final_Metal);

        MTL::RenderCommandEncoder* pEncoder = Window::ins->getCurrentFrameRenderEncoder();

        pEncoder->setRenderPipelineState(m_pCompositeShader->getMetalPipelineState());
        pEncoder->setVertexBuffer(m_pProcessQueue->getMetalFullScreenVertexBuffer(), 0, 0);

        pEncoder->setFragmentTexture(m_pOriginalRenderTexture_Metal, 0);
        pEncoder->setFragmentTexture(m_pRenderTexture_Blur1_Vertical_Metal, 1);
        pEncoder->setFragmentTexture(m_pRenderTexture_Blur2_Vertical_Metal, 2);
        pEncoder->setFragmentSamplerState(MetalRenderer::m_pLinearSampler, 0);

        pEncoder->drawPrimitives(MTL::PrimitiveTypeTriangleStrip, NS::UInteger(0), NS::UInteger(4));
        INCREASE_DRAW_CALL_COUNT(2);

        m_pProcessQueue->setFinalMetalRenderTexture(m_pRenderTexture_Final_Metal);
    }
#endif // __APPLE__
}

void DifferenceOfGaussian::onWindowResize()
{
    m_nRenderWidth = m_pProcessQueue->getRenderWidth();
    m_nRenderHeight = m_pProcessQueue->getRenderHeight();

    int nOneForthWidth = static_cast<int>(m_nRenderWidth * BLUR_TEXTURE_RATIO);
    int nOneForthHeight = static_cast<int>(m_nRenderHeight * BLUR_TEXTURE_RATIO);

    if (Window::ins->isUsingOpenGL())
    {
        initializeRenderTextureAndFBO(m_nFBOID_Blur1_Horizontal, m_nRenderTexture_Blur1_Horizontal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_BLur1_Vertical, m_nRenderTexture_Blur1_Vertical, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_Blur2_Horizontal, m_nRenderTexture_Blur2_Horizontal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_BLur2_Vertical, m_nRenderTexture_Blur2_Vertical, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_nFBOID_Final, m_nRenderTexture_Final, m_nRenderWidth, m_nRenderHeight);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        initializeRenderTextureAndFBO(m_pOriginalRenderTexture_Metal, m_nRenderWidth, m_nRenderHeight);
        initializeRenderTextureAndFBO(m_pRenderTexture_Blur1_Horizontal_Metal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pRenderTexture_Blur1_Vertical_Metal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pRenderTexture_Blur2_Horizontal_Metal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pRenderTexture_Blur2_Vertical_Metal, nOneForthWidth, nOneForthHeight);
        initializeRenderTextureAndFBO(m_pRenderTexture_Final_Metal, m_nRenderWidth, m_nRenderHeight);
    }
#endif
}


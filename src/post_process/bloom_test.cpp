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

    initializeColorHighlightFBO();
    initializeHorizontalBlurFBO();
    initializeVerticalBlurFBO();
    initializeFinalFBO();

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the FBO when done
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind any texture when done

    initializeQuad();
}

void BloomTest::initializeColorHighlightFBO()
{
    glGenFramebuffers(1, &m_nFBOID_ColorHighlight);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_ColorHighlight);

    glGenTextures(1, &m_nRenderTexture_ColorHighlight);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_ColorHighlight);

    // Set the texture's format and size to match your window
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nRenderWidth, m_nRenderHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // Set texture parameters for correct filtering and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Attach the texture to the FBO's color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_nRenderTexture_ColorHighlight, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGERR("Framebuffer is not complete!");
    }
}

void BloomTest::initializeHorizontalBlurFBO()
{
    glGenFramebuffers(1, &m_nFBOID_HorizontalBlur);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_HorizontalBlur);

    glGenTextures(1, &m_nRenderTexture_HorizontalBlur);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_HorizontalBlur);

    // Set the texture's format and size to match your window
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // Set texture parameters for correct filtering and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Attach the texture to the FBO's color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_nRenderTexture_HorizontalBlur, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGERR("Framebuffer is not complete!");
    }
}

void BloomTest::initializeVerticalBlurFBO()
{
    glGenFramebuffers(1, &m_nFBOID_VerticalBlur);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_VerticalBlur);

    glGenTextures(1, &m_nRenderTexture_VerticalBlur);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_VerticalBlur);

    // Set the texture's format and size to match your window
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // Set texture parameters for correct filtering and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Attach the texture to the FBO's color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_nRenderTexture_VerticalBlur, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGERR("Framebuffer is not complete!");
    }
}

void BloomTest::initializeFinalFBO()
{
    glGenFramebuffers(1, &m_nFBOID_Final);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_Final);

    glGenTextures(1, &m_nRenderTexture_Final);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_Final);

    // Set the texture's format and size to match your window
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nRenderWidth, m_nRenderHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // Set texture parameters for correct filtering and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Attach the texture to the FBO's color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_nRenderTexture_Final, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGERR("Framebuffer is not complete!");
    }
}

void BloomTest::initializeQuad()
{
    m_pColorHighlightShader = ShaderLoader::getInstance()->getShader("bloom_filter");
    m_nTextureUniform_ColorHighlight = m_pColorHighlightShader->getUniformLocation("u_tex0");

    m_pHorizontalBlurShader = ShaderLoader::getInstance()->getShader("horizontal_blur");
    m_nTextureUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformLocation("u_tex0");
    m_nTextureWidthUniform_HorizontalBlur = m_pHorizontalBlurShader->getUniformLocation("u_textureWidth");

    m_pVerticalBlurShader = ShaderLoader::getInstance()->getShader("vertical_blur");
    m_nTextureUniform_VerticalBlur = m_pVerticalBlurShader->getUniformLocation("u_tex0");
    m_nTextureHeightUniform_VerticalBlur = m_pVerticalBlurShader->getUniformLocation("u_textureHeight");

    m_pCompositeShader = ShaderLoader::getInstance()->getShader("bloom_composite");
    m_nOriginalTextureUniform = m_pCompositeShader->getUniformLocation("u_originalScreenTexture");
    m_nBloomTextureUniform = m_pCompositeShader->getUniformLocation("u_bloomTexture");
    m_nBloomTextureScaleUniform = m_pCompositeShader->getUniformLocation("u_bloomTextureScale");
    m_nIntensityUniform = m_pCompositeShader->getUniformLocation("u_intensity");

    glBindBuffer(GL_ARRAY_BUFFER, m_pProcessQueue->getFullScreenVertexBuffer());

    GLuint nVPosAttr_ColorHighlight = m_pColorHighlightShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr_ColorHighlight = m_pColorHighlightShader->getAttributeLocation("a_vUV");

    GLuint nVPosAttr_HorizontalBlur = m_pHorizontalBlurShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr_HorizontalBlur = m_pHorizontalBlurShader->getAttributeLocation("a_vUV");

    GLuint nVPosAttr_VerticalBlur = m_pVerticalBlurShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr_VerticalBlur = m_pVerticalBlurShader->getAttributeLocation("a_vUV");

    GLuint nVPosAttr_Composite = m_pCompositeShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr_Composite = m_pCompositeShader->getAttributeLocation("a_vUV");

    // glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());

    glEnableVertexAttribArray(nVPosAttr_ColorHighlight);
    glVertexAttribPointer(nVPosAttr_ColorHighlight, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nVUVAttr_ColorHighlight);
    glVertexAttribPointer(nVUVAttr_ColorHighlight, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    glEnableVertexAttribArray(nVPosAttr_HorizontalBlur);
    glVertexAttribPointer(nVPosAttr_HorizontalBlur, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nVUVAttr_HorizontalBlur);
    glVertexAttribPointer(nVUVAttr_HorizontalBlur, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    glEnableVertexAttribArray(nVPosAttr_VerticalBlur);
    glVertexAttribPointer(nVPosAttr_VerticalBlur, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nVUVAttr_VerticalBlur);
    glVertexAttribPointer(nVUVAttr_VerticalBlur, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    glEnableVertexAttribArray(nVPosAttr_Composite);
    glVertexAttribPointer(nVPosAttr_Composite, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nVUVAttr_Composite);
    glVertexAttribPointer(nVUVAttr_Composite, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BloomTest::renderProcess()
{
    renderColorHighlight();
    renderHorizontalBlur();
    renderVerticalBlur();

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

    glUniform1i(m_nTextureUniform_ColorHighlight, 0); // Texture unit 0

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pProcessQueue->getOriginalRenderTexture());

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

    glUniform1i(m_nTextureUniform_HorizontalBlur, 0); // Texture unit 0
    glUniform1i(m_nTextureWidthUniform_HorizontalBlur, m_nRenderWidth * BLUR_TEXTURE_RATIO);

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

    glUniform1i(m_nTextureUniform_VerticalBlur, 0); // Texture unit 0
    glUniform1i(m_nTextureHeightUniform_VerticalBlur, m_nRenderHeight * BLUR_TEXTURE_RATIO);

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

    glUniform1i(m_nOriginalTextureUniform, 0);
    glUniform1i(m_nBloomTextureUniform, 1);
    glUniform1f(m_nBloomTextureScaleUniform, 1);
    glUniform1f(m_nIntensityUniform, m_nIntensity);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pProcessQueue->getOriginalRenderTexture());

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

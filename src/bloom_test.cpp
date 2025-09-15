#include "bloom_test.h"

#include <glad/gl.h>
#include "window.h"
#include "debug_macro.h"
#include "draw/vertex.h"
#include "draw/shader.h"
#include "draw/shader_loader.h"

#define BLUR_TEXTURE_RATIO 0.25f

void BloomTest::initialize(const Window* pWindow)
{
    m_nRenderWidth = pWindow->GetActualWidth();
    m_nRenderHeight = pWindow->GetActualHeight();

    initializeColorHighlightFBO();
    initializeHorizontalBlurFBO();
    initializeVerticalBlurFBO();

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

    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

    VertexWUV arrVertices[4];
    arrVertices[0] = { { -1, -1 }, { 0.0f, 0.0f } }; // Bottom left
    arrVertices[1] = { { 1, -1 }, { 1.0f, 0.0f } }; // Bottom right
    arrVertices[2] = { { -1, 1 }, { 0.0f, 1.0f } }; // Top left
    arrVertices[3] = { { 1, 1 }, { 1.0f, 1.0f } }; // Top right
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

    GLuint nVPosAttr_ColorHighlight = m_pColorHighlightShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr_ColorHighlight = m_pColorHighlightShader->getAttributeLocation("a_vUV");

    GLuint nVPosAttr_HorizontalBlur = m_pHorizontalBlurShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr_HorizontalBlur = m_pHorizontalBlurShader->getAttributeLocation("a_vUV");

    GLuint nVPosAttr_VerticalBlur = m_pVerticalBlurShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr_VerticalBlur = m_pVerticalBlurShader->getAttributeLocation("a_vUV");

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);

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

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BloomTest::startRenderingGame(const Window* pWindow)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_ColorHighlight);

    glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
    // glViewport(0, 0, m_nActualWidth, m_nActualHeight);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomTest::endRenderingGame(const Window* pWindow)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    renderColorHighlight(pWindow);
    renderHorizontalBlur(pWindow);
    renderVerticalBlur(pWindow);
}

void BloomTest::renderColorHighlight(const Window* pWindow)
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pColorHighlightShader, "Shader must be set before drawing the quad");

    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_HorizontalBlur);
    glViewport(0, 0, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pColorHighlightShader->getProgram());

    glUniform1i(m_nTextureUniform_ColorHighlight, 0); // Texture unit 0

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_ColorHighlight);

    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomTest::renderHorizontalBlur(const Window* pWindow)
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pHorizontalBlurShader, "Shader must be set before drawing the quad");

    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_VerticalBlur);
    glViewport(0, 0, m_nRenderWidth * BLUR_TEXTURE_RATIO, m_nRenderHeight * BLUR_TEXTURE_RATIO);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pHorizontalBlurShader->getProgram());

    glUniform1i(m_nTextureUniform_HorizontalBlur, 0); // Texture unit 0
    glUniform1i(m_nTextureWidthUniform_HorizontalBlur, m_nRenderWidth * BLUR_TEXTURE_RATIO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_HorizontalBlur);

    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomTest::renderVerticalBlur(const Window* pWindow)
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pVerticalBlurShader, "Shader must be set before drawing the quad");

    // glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_VerticalBlur);
    glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pVerticalBlurShader->getProgram());

    glUniform1i(m_nTextureUniform_VerticalBlur, 0); // Texture unit 0
    glUniform1i(m_nTextureHeightUniform_VerticalBlur, m_nRenderHeight * BLUR_TEXTURE_RATIO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_VerticalBlur);

    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


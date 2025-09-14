#include "bloom_test.h"

#include <glad/gl.h>
#include "window.h"
#include "debug_macro.h"
#include "draw/vertex.h"
#include "draw/shader.h"
#include "draw/shader_loader.h"

void BloomTest::initialize(const Window* pWindow)
{
    initializeColorHighlightFBO(pWindow);
    initializeQuad();
}

void BloomTest::initializeColorHighlightFBO(const Window* pWindow)
{
    glGenFramebuffers(1, &m_nFBOID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID);

    glGenTextures(1, &m_nRenderTexture);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture);

    // Set the texture's format and size to match your window
    m_nRenderWidth = pWindow->GetActualWidth();
    m_nRenderHeight = pWindow->GetActualHeight();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nRenderWidth, m_nRenderHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // Set texture parameters for correct filtering and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Attach the texture to the FBO's color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_nRenderTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGERR("Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the FBO when done
}

void BloomTest::initializeQuad()
{
    m_pShader = ShaderLoader::getInstance()->getShader("bloom_filter");

    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

    VertexWUV arrVertices[4];
    arrVertices[0] = { { -1, -1 }, { 0.0f, 0.0f } }; // Bottom left
    arrVertices[1] = { { 1, -1 }, { 1.0f, 0.0f } }; // Bottom right
    arrVertices[2] = { { -1, 1 }, { 0.0f, 1.0f } }; // Top left
    arrVertices[3] = { { 1, 1 }, { 1.0f, 1.0f } }; // Top right
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

    m_nTextureUniform = m_pShader->getUniformLocation("u_tex0");
    GLuint nVPosAttr = m_pShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr = m_pShader->getAttributeLocation("a_vUV");

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nVUVAttr);
    glVertexAttribPointer(nVUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BloomTest::startRenderingFBO(const Window* pWindow)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID);

    glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
    // glViewport(0, 0, m_nActualWidth, m_nActualHeight);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomTest::endRenderingFBO(const Window* pWindow)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, pWindow->GetActualWidth(), pWindow->GetActualHeight());
    glClear(GL_COLOR_BUFFER_BIT);



    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    glUseProgram(m_pShader->getProgram());

    glUniform1i(m_nTextureUniform, 0); // Texture unit 0

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture);

    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

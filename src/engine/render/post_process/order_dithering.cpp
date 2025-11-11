#include "order_dithering.h"

#include <glad/glad.h>
#include "../shader_loader.h"
#include "../../core/debug_macro.h"
#include "../../core/window.h"


void OrderDithering::initialize()
{
    m_nRenderWidth = m_pProcessQueue->getRenderWidth();
    m_nRenderHeight = m_pProcessQueue->getRenderHeight();

    initializeRenderTextureAndFBO(m_nFBOID, m_nRenderTexture, m_nRenderWidth, m_nRenderHeight);

    initializeShader();
}

void OrderDithering::initializeShader()
{
    m_pShader = ShaderLoader::getInstance()->getShader("order_dithering");
    m_pTextureUniform = m_pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);

    glBindBuffer(GL_ARRAY_BUFFER, m_pProcessQueue->getFullScreenVertexBuffer());
    glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());

    registerShaderPosAndUV(m_pShader);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OrderDithering::renderProcess()
{
    render();
}

void OrderDithering::render()
{
    // Debug draw it to screen instead of another post-process render texture
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID);
    glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pShader->getProgram());

    glUniform1i(m_pTextureUniform->m_nLocation, 0); // Texture unit 0

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pProcessQueue->getFinalRenderTexture());

    glBindVertexArray(m_pProcessQueue->getFullScreenVertexArray());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_pProcessQueue->setFinalRenderTexture(m_nRenderTexture);
}

#include "text_renderer.h"

#include <glad/gl.h>

#include "../../core/camera.h"
#include "../../core/scene/node.h"
#include "../../render/vertex.h"
#include "../../render/image_loader.h"
#include "../../render/shader_loader.h"
#include "../../render/font/font_loader.h"
#include "../../render/core/renderer.h"

TextRenderer::TextRenderer(Font* const pFont) : m_pFont(pFont)
{
}

TextRenderer::~TextRenderer()
{
}

void TextRenderer::setShader(Shader* pShader)
{
    m_pShader = pShader;

    if (m_pShader)
    {
        m_pMVPHandle = m_pShader->getUniformHandle("u_MVP");
        m_pTextureHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
        m_pSizeHandle = m_pShader->getUniformHandle("u_Size");

        bindVertexArray(m_pShader);
    }
}

void TextRenderer::bindVertexArray(Shader* const pShader)
{
    VertexWUV arrVertices[4];
    arrVertices[0] = { { -0.5, -0.5 }, { 0.0f, 0.0f } }; // Bottom left
    arrVertices[1] = { { 0.5, -0.5 }, { 1.0f, 0.0f } }; // Bottom right
    arrVertices[2] = { { -0.5, 0.5 }, { 0.0f, 1.0f } }; // Top right
    arrVertices[3] = { { 0.5, 0.5 }, { 1.0f, 1.0f } }; // Top left

    if (Renderer::isUsingOpenGL())
    {
        glGenVertexArrays(1, &m_nVertexArray);
        glGenBuffers(1, &m_nVertexBuffer);

        glBindVertexArray(m_nVertexArray);

        glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

        GLint nPosAttribLocation = pShader->getAttributeLocation("a_vPos");
        glEnableVertexAttribArray(nPosAttribLocation);
        glVertexAttribPointer(nPosAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));

        GLint nUVAttribLocation = pShader->getAttributeLocation("a_vUV");
        glEnableVertexAttribArray(nUVAttribLocation);
        glVertexAttribPointer(nUVAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void TextRenderer::draw()
{
    if (m_strText.empty()) return;

    mat4x4 mvp;
    const mat4x4& matModel = m_pNode->getWorldMatrix();
    const mat4x4& cameraViewMatrix = Camera::main->getViewProjectionMatrix();
    mat4x4_mul(mvp, cameraViewMatrix, matModel);

    if (Renderer::isUsingOpenGL())
    {
        glUseProgram(m_pShader->getProgram());
        
        glBindVertexArray(m_nVertexArray);
        glUniform1i(m_pTextureHandle->m_nLocation, 0);
        glActiveTexture(GL_TEXTURE0);

        float fScale = 1.f / 64.f;
        float fXOffset = 0.f;
        
        for (int i = 0; i < m_strText.size(); i++)
        {
            Character* pCharacter = m_pFont->getCharacter(m_strText[i]);
            if (pCharacter)
            {
                float fGlyphWidth = pCharacter->m_vecSize[0] * fScale;
                float fGlyphHeight = pCharacter->m_vecSize[1] * fScale;
                float fBearingX = pCharacter->m_vecBearing[0] * fScale;
                float fBearingY = pCharacter->m_vecBearing[1] * fScale;

                glUniform2f(m_pSizeHandle->m_nLocation, fGlyphWidth, fGlyphHeight);

                mat4x4 finalMVP;
                // Position glyph so its baseline aligns via bearing and size (quad is centered)
                float fGlyphCenterX = fXOffset + fBearingX + (fGlyphWidth * 0.5f);
                float fGlyphCenterY = fBearingY - (fGlyphHeight * 0.5f);
                mat4x4_translate(finalMVP, fGlyphCenterX, fGlyphCenterY, 0);
                mat4x4_mul(finalMVP, mvp, finalMVP);

                glUniformMatrix4fv(m_pMVPHandle->m_nLocation, 1, GL_FALSE, (const GLfloat*) finalMVP);
                
                glBindTexture(GL_TEXTURE_2D, pCharacter->m_nTextureID);

                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                INCREASE_DRAW_CALL_COUNT(2);

                // Advance in FreeType is stored in 1/64th pixels
                fXOffset += (pCharacter->m_nAdvance >> 6) * fScale;
            }
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }
}

void TextRenderer::setMaterial(const std::shared_ptr<Material>& pMaterial)
{
    m_pMaterial = pMaterial;
}

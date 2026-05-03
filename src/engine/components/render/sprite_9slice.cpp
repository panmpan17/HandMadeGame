#include "sprite_9slice.h"

#include <glad/gl.h>

#include "../../core/camera.h"
#include "../../core/scene/node.h"
#include "../../core/debug_macro.h"
#include "../../render/image.h"
#include "../../render/shader_loader.h"
#include "../../render/core/renderer.h"
#include "../../render/vertex.h"

Sprite9Slice::Sprite9Slice(Image* pImage, float fWidth, float fHeight, float fPixelPerUnit, const Slice9& slice, int nPixelPerUnit)
    : m_pImage(pImage), m_fWidth(fWidth), m_fHeight(fHeight), m_fPixelPerUnit(fPixelPerUnit), m_slice(slice)
{
    m_pShader = ShaderLoader::getInstance()->getShader("sprite_9slice");

    m_pMVPHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_MVP);
    m_pTextureHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
}

Sprite9Slice::~Sprite9Slice()
{
    if (Renderer::isUsingOpenGL())
    {
        glDeleteBuffers(1, &m_nVertexBuffer);
        glDeleteVertexArrays(1, &m_nVertexArray);
    }
}

void Sprite9Slice::registerBuffer()
{
    const float fLeftEdgePercentage = m_slice.fPixelOnLeftEdge / m_pImage->getWidth();
    const float fRightEdgePercentage = 1 - (m_slice.fPixelOnRightEdge / m_pImage->getWidth());
    const float fBottomEdgePercentage = m_slice.fPixelOnBottomEdge / m_pImage->getHeight();
    const float fTopEdgePercentage = 1 - (m_slice.fPixelOnTopEdge / m_pImage->getHeight());

    float fVerticesX[4] = { -m_fWidth / 2.0f, -m_fWidth / 2.0f + m_slice.fPixelOnLeftEdge / m_fPixelPerUnit, m_fWidth / 2.0f - m_slice.fPixelOnRightEdge / m_fPixelPerUnit, m_fWidth / 2.0f };
    float fVerticesY[4] = { -m_fHeight / 2.0f, -m_fHeight / 2.0f + m_slice.fPixelOnBottomEdge / m_fPixelPerUnit, m_fHeight / 2.0f - m_slice.fPixelOnTopEdge / m_fPixelPerUnit, m_fHeight / 2.0f };
    float fUVsX[4] = { 0.0f, fLeftEdgePercentage, fRightEdgePercentage, 1.0f };
    float fUVsY[4] = { 0.0f, fBottomEdgePercentage, fTopEdgePercentage, 1.0f };

    VertexWUV arrVertices[16];
    int nIndex = 0;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            arrVertices[nIndex].pos[0] = fVerticesX[j];
            arrVertices[nIndex].pos[1] = fVerticesY[i];
            arrVertices[nIndex].uv[0] = fUVsX[j];
            arrVertices[nIndex].uv[1] = fUVsY[i];
            ++nIndex;
        }
    }

    VertexWUV vecVertices[54];
    int nVertexIndex = 0;
    std::function<void(const VertexWUV&, const VertexWUV&, const VertexWUV&, const VertexWUV&)> addQuad = [&vecVertices, &nVertexIndex](const VertexWUV& v1, const VertexWUV& v2, const VertexWUV& v3, const VertexWUV& v4)
    {
        vecVertices[nVertexIndex++] = v1;
        vecVertices[nVertexIndex++] = v2;
        vecVertices[nVertexIndex++] = v3;

        vecVertices[nVertexIndex++] = v4;
        vecVertices[nVertexIndex++] = v3;
        vecVertices[nVertexIndex++] = v2;
    };
    
    // Bottom row
    addQuad(arrVertices[0], arrVertices[1], arrVertices[4], arrVertices[5]);
    addQuad(arrVertices[1], arrVertices[2], arrVertices[5], arrVertices[6]);
    addQuad(arrVertices[2], arrVertices[3], arrVertices[6], arrVertices[7]);

    // Middle row
    addQuad(arrVertices[4], arrVertices[5], arrVertices[8], arrVertices[9]);
    addQuad(arrVertices[5], arrVertices[6], arrVertices[9], arrVertices[10]);
    addQuad(arrVertices[6], arrVertices[7], arrVertices[10], arrVertices[11]);

    // Top row
    addQuad(arrVertices[8], arrVertices[9], arrVertices[12], arrVertices[13]);
    addQuad(arrVertices[9], arrVertices[10], arrVertices[13], arrVertices[14]);
    addQuad(arrVertices[10], arrVertices[11], arrVertices[14], arrVertices[15]);
    
    

    if (Renderer::isUsingOpenGL())
    {
        if (m_nVertexBuffer == GL_INVALID_INDEX)
        {
            glGenBuffers(1, &m_nVertexBuffer);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vecVertices), vecVertices, GL_STATIC_DRAW);

        GLuint nVPosAttr = m_pShader->getAttributeLocation("a_vPos");
        GLuint nVUVAttr = m_pShader->getAttributeLocation("a_vUV");

        if (m_nVertexArray == GL_INVALID_INDEX)
        {
            glGenVertexArrays(1, &m_nVertexArray);
            glBindVertexArray(m_nVertexArray);
            glEnableVertexAttribArray(nVPosAttr);
            glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
            glEnableVertexAttribArray(nVUVAttr);
            glVertexAttribPointer(nVUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));
            glBindVertexArray(0);
        }

        // Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Sprite9Slice::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    if (m_bBufferDirty)
    {
        registerBuffer();
        m_bBufferDirty = false;
    }

    mat4x4 mvp;
    const mat4x4& matModel = m_pNode->getWorldMatrix();
    const mat4x4& cameraViewMatrix = Camera::main->getViewProjectionMatrix();
    mat4x4_mul(mvp, cameraViewMatrix, matModel);

    if (Renderer::isUsingOpenGL())
    {
        glUseProgram(m_pShader->getProgram());
        glUniformMatrix4fv(m_pMVPHandle->m_nLocation, 1, GL_FALSE, (const GLfloat*) mvp);
        // glUniform4f(m_pColorHandle->m_nLocation, m_color[0], m_color[1], m_color[2], m_color[3]);

        // glUniform1i(m_pSpriteSheetXCountHandle->m_nLocation, m_vecSpriteSheetCount.x);
        // glUniform1i(m_pSpriteSheetYCountHandle->m_nLocation, m_vecSpriteSheetCount.y);
        // glUniform2f(m_pUVOffsetHandle->m_nLocation, m_vecUVOOffset[0], m_vecUVOOffset[1]);

        if (m_pImage)
        {
            glUniform1i(m_pTextureHandle->m_nLocation, 0); // Texture unit 0

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_pImage->getTextureID());
        }
        else
        {
            LOGLN("Warning: Sprite9Slice does not have an image set, it will be rendered as white");
        }

        glBindVertexArray(m_nVertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 54); // Draw the quad using triangle strip
        INCREASE_DRAW_CALL_COUNT(18);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        glBindVertexArray(0); // Unbind the vertex array
        glUseProgram(0);
    }
}
#include "point_light.h"

#include <glad/gl.h>
#include "light_manager.h"
#include "../debug_macro.h"
#include "../draw/shader_loader.h"
#include "../camera.h"
#include "../draw/vertex.h"
#include "../window.h"


PointLightComponent::PointLightComponent()
{
    LightManager::getInstance()->registerPointLightComponent(this);
}

PointLightComponent::~PointLightComponent()
{
    LightManager::getInstance()->unregisterPointLightComponent(this);
}

void PointLightComponent::onAddToNode()
{
    if (!m_pNode)
    {
        return;
    }

    m_pNode->registerOnPositionChangedListener(std::bind(&PointLightComponent::markLightDataDirty, this));

    
    m_pShader = ShaderLoader::getInstance()->getShader("point_light");
    m_pMVPUniformHandle = m_pShader->getUniformHandle("u_MVP");
    m_pLightColorUniformHandle = m_pShader->getUniformHandle("u_lightColor");


    registerBuffer();
}


void PointLightComponent::registerBuffer()
{
    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

    float fStartX = -1 / 2.0f;
    float fStartY = -1 / 2.0f;
    VertexWUV arrVertices[4];
    arrVertices[0] = { { fStartX, fStartY }, { 0.0f, 0.0f } }; // Bottom left
    arrVertices[1] = { { fStartX + 1, fStartY }, { 1.0f, 0.0f } }; // Bottom right
    arrVertices[2] = { { fStartX, fStartY + 1 }, { 0.0f, 1.0f } }; // Top right
    arrVertices[3] = { { fStartX + 1, fStartY + 1 }, { 1.0f, 1.0f } }; // Top left
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

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

void PointLightComponent::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    mat4x4 mvp;
    const mat4x4& matModel = m_pNode->getWorldMatrix();
    const mat4x4& cameraViewMatrix = Camera::main->getViewProjectionMatrix();
    mat4x4_mul(mvp, cameraViewMatrix, matModel);

    glUseProgram(m_pShader->getProgram());
    if (m_pMVPUniformHandle)
    {
        glUniformMatrix4fv(m_pMVPUniformHandle->m_nLocation, 1, GL_FALSE, (const GLfloat*) mvp);
    }
    if (m_pLightColorUniformHandle)
    {
        glUniform4f(m_pLightColorUniformHandle->m_nLocation, m_color[0], m_color[1], m_color[2], 1);
    }

    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

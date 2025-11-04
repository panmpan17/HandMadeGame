#include "point_light.h"

#include <glad/gl.h>
#include "light_manager.h"
#include "../debug_macro.h"
#include "../draw/shader_loader.h"
#include "../camera.h"
#include "../draw/vertex.h"
#include "../window.h"


constexpr float POINT_LIGHT_SIZE = 0.3f;


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

    float fStartX = -POINT_LIGHT_SIZE / 2.0f;
    float fStartY = -POINT_LIGHT_SIZE / 2.0f;
    VertexWUV arrVertices[4];
    arrVertices[0] = { { fStartX, fStartY } }; // Bottom left
    arrVertices[1] = { { fStartX + POINT_LIGHT_SIZE, fStartY } }; // Bottom right
    arrVertices[2] = { { fStartX, fStartY + POINT_LIGHT_SIZE } }; // Top right
    arrVertices[3] = { { fStartX + POINT_LIGHT_SIZE, fStartY + POINT_LIGHT_SIZE } }; // Top left
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

    GLuint nVPosAttr = m_pShader->getAttributeLocation("a_vPos");

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));

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
        glUniform3f(m_pLightColorUniformHandle->m_nLocation, m_color[0] * m_intensity, m_color[1] * m_intensity, m_color[2] * m_intensity);
    }

    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

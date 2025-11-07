#include "direction_light.h"

#include <glad/gl.h>
#include "light_manager.h"
#include "../vertex.h"
#include "../shader.h"
#include "../shader_loader.h"
#include "../../core/window.h"
#include "../../core/camera.h"
#include "../../core/debug_macro.h"


constexpr float LIGHT_SIZE = 10.f;


DirectionLightComponent::DirectionLightComponent()
{
    LightManager::getInstance()->registerDirectionLightComponent(this);
}

DirectionLightComponent::~DirectionLightComponent()
{
    LightManager::getInstance()->unregisterDirectionLightComponent(this);
}

void DirectionLightComponent::onAddToNode()
{
    if (!m_pNode)
    {
        return;
    }

    m_pNode->registerOnPositionChangedListener(std::bind(&DirectionLightComponent::markLightDataDirty, this));

    m_pShader = ShaderLoader::getInstance()->getShader("point_light");
    m_pMVPUniformHandle = m_pShader->getUniformHandle("u_MVP");
    m_pLightColorUniformHandle = m_pShader->getUniformHandle("u_lightColor");

    registerBuffer();
}

void DirectionLightComponent::registerBuffer()
{
    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

    float fStartX = -LIGHT_SIZE / 2.0f;
    float fStartY = -LIGHT_SIZE / 2.0f;
    VertexWUV arrVertices[4];
    arrVertices[0] = { { fStartX, fStartY } }; // Bottom left
    arrVertices[1] = { { fStartX, fStartY + LIGHT_SIZE } }; // Top right
    arrVertices[2] = { { fStartX + LIGHT_SIZE, fStartY } }; // Bottom right
    arrVertices[3] = { { fStartX + LIGHT_SIZE, fStartY + LIGHT_SIZE } }; // Top left
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

void DirectionLightComponent::draw()
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

const mat4x4& DirectionLightComponent::getLightCastingMatrix()
{
    if (m_bLightCastingMatrixDirty)
    {
        // For directional light, we can create an orthographic projection matrix
        float orthoSize = 15.0f; // Size of the orthographic box
        float nearPlane = -10.0f;
        float farPlane = 30.0f;

        mat4x4 matOrtho;
        mat4x4_ortho(matOrtho, -orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);

        mat4x4 matView;

        const Vector3& vecPos = m_pNode->getPositionInWorld();
        vec3 camForward, camUp;
        Quaternion rotation = m_pNode->getWorldRotationQuaternion();
        rotation.getForwardVector(camForward);
        rotation.getUpVector(camUp);
        camForward[0] += vecPos.x;
        camForward[1] += vecPos.y;
        camForward[2] += vecPos.z;
        mat4x4_look_at(matView, reinterpret_cast<const vec3&>(vecPos), camForward, camUp);

        // Combine to get the light casting matrix
        mat4x4_mul(m_matLightCastingMatrix, matOrtho, matView);

        m_bLightCastingMatrixDirty = false;
    }

    return m_matLightCastingMatrix;
}

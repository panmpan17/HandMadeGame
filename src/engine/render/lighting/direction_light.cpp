#include "direction_light.h"

#include <glad/gl.h>
#include "light_manager.h"
#include "../vertex.h"
#include "../shader.h"
#include "../shader_loader.h"
#include "../image_loader.h"
#include "../../core/window.h"
#include "../../core/camera.h"
#include "../../core/debug_macro.h"
#include "../../core/serialization/serializer.h"


constexpr float GIZMOS_SIZE = 2.f;

Image* pDirectionLightGizmosImage = nullptr;

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

    m_pNode->registerOnRotationChangedListener(std::bind(&DirectionLightComponent::markLightDataDirty, this));

    m_pShader = ShaderLoader::getInstance()->getShader("simple_gizmos");
    m_pPositionUniform = m_pShader->getUniformHandle("u_WorldPosition");
    m_pColorUniform = m_pShader->getUniformHandle("u_imageColor");
    m_pTextureUniform = m_pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
    m_pUseTextureUniform = m_pShader->getUniformHandle("u_useTexture");

    registerBuffer();

    if (!pDirectionLightGizmosImage)
    {
        pDirectionLightGizmosImage = ImageLoader::getInstance()->getImageByPath("assets/gizmos/direction_light.png");
    }
}


bool DirectionLightComponent::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    if (Component::deserializeField(deserializer, strFieldName, strFieldValue)) return true;

    DESERIALIZE_FIELD(m_color);
    DESERIALIZE_FIELD(m_intensity);
    DESERIALIZE_FIELD(m_bEnableShadows);
    DESERIALIZE_FIELD(m_shadowColor);
    DESERIALIZE_FIELD(m_fShadowIntensity);

    return false;
}

void DirectionLightComponent::serializeToWrapper(DataSerializer& serializer) const
{
    serializer.ADD_ATTRIBUTES(m_color);
    serializer.ADD_ATTRIBUTES(m_intensity);
    serializer.ADD_ATTRIBUTES(m_bEnableShadows);
    serializer.ADD_ATTRIBUTES(m_shadowColor);
    serializer.ADD_ATTRIBUTES(m_fShadowIntensity);
}

void DirectionLightComponent::onNodeFinishedDeserialization()
{
    // TODO: does this need to trigger on add to node?
}


void DirectionLightComponent::registerBuffer()
{
    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

    float fStartX = -GIZMOS_SIZE / 2.0f;
    float fStartY = -GIZMOS_SIZE / 2.0f;
    VertexWUV arrVertices[4];
    arrVertices[0] = { { fStartX, fStartY }, { 0, 0}  }; // Bottom left
    arrVertices[2] = { { fStartX, fStartY + GIZMOS_SIZE }, { 0, 1 } }; // Top right
    arrVertices[1] = { { fStartX + GIZMOS_SIZE, fStartY }, { 1, 0 } }; // Bottom right
    arrVertices[3] = { { fStartX + GIZMOS_SIZE, fStartY + GIZMOS_SIZE }, { 1, 1 } }; // Top left
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

    GLuint nVPosAttr = m_pShader->getAttributeLocation("a_vPos");
    GLuint nUVAttr = m_pShader->getAttributeLocation("a_vUV");

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nUVAttr);
    glVertexAttribPointer(nUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DirectionLightComponent::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    const mat4x4& matModel = m_pNode->getWorldMatrix();

    glUseProgram(m_pShader->getProgram());
    if (m_pPositionUniform)
    {
        const Vector3& worldPos = m_pNode->getPositionInWorld();
        glUniform3f(m_pPositionUniform->m_nLocation, worldPos.x, worldPos.y, worldPos.z);
    }
    if (m_pColorUniform)
    {
        glUniform4f(m_pColorUniform->m_nLocation, m_color.x, m_color.y, m_color.z, 1);
    }
    if (m_pUseTextureUniform)
    {
        glUniform1i(m_pUseTextureUniform->m_nLocation, pDirectionLightGizmosImage ? 1 : 0); // true
    }
    if (m_pTextureUniform && pDirectionLightGizmosImage)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pDirectionLightGizmosImage->getTextureID());
        glUniform1i(m_pTextureUniform->m_nLocation, 0);
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

#include "point_light.h"

#include <glad/gl.h>
#include "light_manager.h"
#include "../shader_loader.h"
#include "../vertex.h"
#include "../../core/window.h"
#include "../../core/camera.h"
#include "../../core/debug_macro.h"
#include "../../core/serialization/serializer.h"


constexpr float POINT_LIGHT_SIZE = 0.3f;


PointLightComponent::PointLightComponent()
{
    LightManager::getInstance()->registerPointLightComponent(this);
}

PointLightComponent::~PointLightComponent()
{
    LightManager::getInstance()->unregisterPointLightComponent(this);
}

void PointLightComponent::onStart()
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


bool PointLightComponent::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    if (NodeComponent::deserializeField(deserializer, strFieldName, strFieldValue)) return true;

    DESERIALIZE_FIELD(m_fRange);
    DESERIALIZE_FIELD(m_color);
    DESERIALIZE_FIELD(m_intensity);
    DESERIALIZE_FIELD(m_fAttenuationConstant);
    DESERIALIZE_FIELD(m_fAttenuationLinear);
    DESERIALIZE_FIELD(m_fAttenuationQuadratic);

    return false;
}

void PointLightComponent::serializeToWrapper(DataSerializer& serializer) const
{
    serializer.ADD_ATTRIBUTES(m_fRange);
    serializer.ADD_ATTRIBUTES(m_color);
    serializer.ADD_ATTRIBUTES(m_intensity);
    serializer.ADD_ATTRIBUTES(m_fAttenuationConstant);
    serializer.ADD_ATTRIBUTES(m_fAttenuationLinear);
    serializer.ADD_ATTRIBUTES(m_fAttenuationQuadratic);
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

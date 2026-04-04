#include "point_light.h"

#include <glad/gl.h>
#include "light_manager.h"
#include "../shader_loader.h"
#include "../vertex.h"
#include "../../core/window.h"
#include "../../core/camera.h"
#include "../../core/debug_macro.h"
#include "../../core/serialization/serializer.h"
#include "../../../editor/gizmos.h"


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
}


bool PointLightComponent::deserializeField(DataDeserializer& deserializer, std::string_view strFieldName, std::string_view strFieldValue)
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

inline constexpr std::string_view POINT_LIGHT_GIZMOS_IMAGE = "assets/gizmos/point_light.png";

void PointLightComponent::onDrawGizmos(bool bIsSelected)
{
    GizmosManager::getInstance()->addImageGizmos(this, m_pNode->getPositionInWorld(), POINT_LIGHT_GIZMOS_IMAGE, m_color);
}

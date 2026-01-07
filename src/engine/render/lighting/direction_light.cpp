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
#include "../../../editor/inspector_helper.h"
#include "../../../editor/gizmos.h"



Image* pDirectionLightGizmosImage = nullptr;

DirectionLightComponent::DirectionLightComponent()
{
    LightManager::getInstance()->registerDirectionLightComponent(this);
}

DirectionLightComponent::~DirectionLightComponent()
{
    LightManager::getInstance()->unregisterDirectionLightComponent(this);
}

void DirectionLightComponent::onStart()
{
    if (!m_pNode)
    {
        return;
    }

    m_pNode->registerOnRotationChangedListener(std::bind(&DirectionLightComponent::markLightDataDirty, this));
}


bool DirectionLightComponent::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    if (NodeComponent::deserializeField(deserializer, strFieldName, strFieldValue)) return true;

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

void DirectionLightComponent::onInspectorUI(int nComponentIndex)
{
    bool bModified = false;
    bModified |= inspectorColorField(nComponentIndex, "Color", m_color);
    bModified |= inspectorFloatField(nComponentIndex, "Intensity", m_intensity);

    inspectorBoolField(nComponentIndex, "Shadows Enabled", m_bEnableShadows);
    if (m_bEnableShadows)
    {
        inspectorColorField(nComponentIndex, "Shadow Color", m_shadowColor);
        inspectorFloatField(nComponentIndex, "Shadow Intensity", m_fShadowIntensity);
    }

    if (bModified)
    {
        m_bLightDataDirty = true;
    }
}

inline constexpr std::string_view DIRECTION_LIGHT_GIZMOS_IMAGE = "assets/gizmos/direction_light.png";

void DirectionLightComponent::onDrawGizmos(bool bIsSelected)
{
    GizmosManager::getInstance()->addImageGizmos(this, m_pNode->getPositionInWorld(), DIRECTION_LIGHT_GIZMOS_IMAGE, m_color);
}

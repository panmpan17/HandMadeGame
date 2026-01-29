#include "direction_light.h"

#include <glad/gl.h>
#include "light_manager.h"
#include "../../core/serialization/serializer.h"
#include "../../core/window.h"
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

void mat4x4_ortho_metal(mat4x4 M, float l, float r, float b, float t, float n, float f) 
{
    float ral = r + l;
    float rsl = r - l;
    float tab = t + b;
    float tsb = t - b;
    float fan = f + n;
    float fsn = f - n;

    M[0][0] = 2.0f / rsl;
    M[0][1] = 0.0f;
    M[0][2] = 0.0f;
    M[0][3] = 0.0f;

    M[1][0] = 0.0f;
    M[1][1] = 2.0f / tsb;
    M[1][2] = 0.0f;
    M[1][3] = 0.0f;

    M[2][0] = 0.0f;
    M[2][1] = 0.0f;
    M[2][2] = 1.0f / (f - n); // <--- Crucial Difference: 1.0, not -2.0
    M[2][3] = 0.0f;

    M[3][0] = -ral / rsl;
    M[3][1] = -tab / tsb;
    M[3][2] = -n / (f - n);   // <--- Crucial Difference
    M[3][3] = 1.0f;
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
        if (Window::ins->isUsingOpenGL())
            mat4x4_ortho(matOrtho, -orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
        else
            mat4x4_ortho_metal(matOrtho, -orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);

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

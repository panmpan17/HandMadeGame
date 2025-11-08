#pragma once


#include "light_data.h"
#include "../../components/component.h"
#include "../../core/math/vector.h"
#include "../../core/scene/node.h"


typedef unsigned int GLuint;

class Shader;
class ShaderUniformHandle;


class DirectionLightComponent : public Component
{
public:
    DirectionLightComponent();
    ~DirectionLightComponent();

    inline const Vector3& getColor() const { return m_color; }
    inline void setColor(const Vector3& color) { m_color.x = color.x; m_color.y = color.y; m_color.z = color.z; m_bLightDataDirty = true; }

    inline float getIntensity() const { return m_intensity; }
    inline void setIntensity(float intensity) { m_intensity = intensity; m_bLightDataDirty = true; }


    inline void updateLightData(DirectionLightGPUData* pOutLightData)
    {
        vec3 vecForward;
        getNode()->getRotationQuaternion().getForwardVector(vecForward);
        pOutLightData->m_vecDirection[0] = vecForward[0];
        pOutLightData->m_vecDirection[1] = vecForward[1];
        pOutLightData->m_vecDirection[2] = vecForward[2];

        pOutLightData->m_vecColor[0] = m_color.x * m_intensity;
        pOutLightData->m_vecColor[1] = m_color.y * m_intensity;
        pOutLightData->m_vecColor[2] = m_color.z * m_intensity;

        m_bLightDataDirty = false;
    }

    const mat4x4& getLightCastingMatrix();

    bool isLightDataDirty() const { return m_bLightDataDirty; }
    void markLightDataDirty() { m_bLightDataDirty = true; m_bLightCastingMatrixDirty = true; }

    void onAddToNode() override;
    // void onStart() override {}
    void draw() override;
    void update(float deltaTime) override {}

    bool isIDrawable() const override { return true; }
    bool isUpdatable() const override { return false; }


private:
    Vector3 m_color;
    float m_intensity;

    bool m_bLightDataDirty = true;

    mat4x4 m_matLightCastingMatrix;
    bool m_bLightCastingMatrixDirty = true;

    Shader* m_pShader = nullptr;
    const ShaderUniformHandle* m_pMVPUniformHandle = nullptr;
    const ShaderUniformHandle* m_pLightColorUniformHandle = nullptr;

    GLuint m_nPointLightVAO = 0;
    GLuint m_nPointLightVBO = 0;

    GLuint m_nVertexBuffer = 0;
    GLuint m_nVertexArray = 0;
    void registerBuffer();
};
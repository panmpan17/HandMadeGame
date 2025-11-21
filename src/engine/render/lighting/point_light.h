#pragma once


#include "light_data.h"
#include "../shader.h"
#include "../../components/component.h"
#include "../../core/math/vector.h"
#include "../../core/scene/node.h"

typedef unsigned int GLuint;


class PointLightComponent : public Component
{
public:
    PointLightComponent();
    ~PointLightComponent();


    inline float getRange() const { return m_fRange; }
    inline void setRange(float range) { m_fRange = range; m_bLightDataDirty = true; }

    inline const vec3& getColor() const { return m_color; }
    inline void setColor(const vec3& color) { m_color[0] = color[0]; m_color[1] = color[1]; m_color[2] = color[2]; m_bLightDataDirty = true; }

    inline float getIntensity() const { return m_intensity; }
    inline void setIntensity(float intensity) { m_intensity = intensity; m_bLightDataDirty = true; }

    inline float getAttenuationConstant() const { return m_fAttenuationConstant; }
    inline void setAttenuationConstant(float constant) { m_fAttenuationConstant = constant; m_bLightDataDirty = true; }

    inline float getAttenuationLinear() const { return m_fAttenuationLinear; }
    inline void setAttenuationLinear(float linear) { m_fAttenuationLinear = linear; m_bLightDataDirty = true; }

    inline float getAttenuationQuadratic() const { return m_fAttenuationQuadratic; }
    inline void setAttenuationQuadratic(float quadratic) { m_fAttenuationQuadratic = quadratic; m_bLightDataDirty = true; }


    inline void updateLightData(PointLightGPUData* pOutLightData)
    {
        const Vector3& vecWorldPos = getNode()->getPosition();
        pOutLightData->m_vecPositionAndRange[0] = vecWorldPos.x;
        pOutLightData->m_vecPositionAndRange[1] = vecWorldPos.y;
        pOutLightData->m_vecPositionAndRange[2] = vecWorldPos.z;
        pOutLightData->m_vecPositionAndRange[3] = m_fRange;

        pOutLightData->m_vecColor[0] = m_color[0] * m_intensity;
        pOutLightData->m_vecColor[1] = m_color[1] * m_intensity;
        pOutLightData->m_vecColor[2] = m_color[2] * m_intensity;

        pOutLightData->m_vecAttenuationParams[0] = m_fAttenuationConstant;
        pOutLightData->m_vecAttenuationParams[1] = m_fAttenuationLinear;
        pOutLightData->m_vecAttenuationParams[2] = m_fAttenuationQuadratic;

        m_bLightDataDirty = false;
    }

    bool isLightDataDirty() const { return m_bLightDataDirty; }
    void markLightDataDirty() { m_bLightDataDirty = true; }

    void onAddToNode() override;
    // void onStart() override {}
    void draw() override;
    void update(float deltaTime) override {}

    bool isIDrawable() const override { return true; }
    bool isUpdatable() const override { return false; }


private:
    float m_fRange = 10.f;
    vec3 m_color;
    float m_intensity;
    float m_fAttenuationConstant = 1.f;
    float m_fAttenuationLinear = 0.09f;
    float m_fAttenuationQuadratic = 0.032f;

    bool m_bLightDataDirty = true;

    Shader* m_pShader = nullptr;
    const ShaderUniformHandle* m_pMVPUniformHandle = nullptr;
    const ShaderUniformHandle* m_pLightColorUniformHandle = nullptr;

    GLuint m_nPointLightVAO = 0;
    GLuint m_nPointLightVBO = 0;

    GLuint m_nVertexBuffer = 0;
    GLuint m_nVertexArray = 0;
    void registerBuffer();

    COMPONENT_REGISTER_SERIALIZABLE(PointLightComponent)
};

REGISTER_CLASS(PointLightComponent)

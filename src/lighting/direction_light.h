#pragma once


#include "light_data.h"
#include "../components/component.h"
#include "../vector.h"
#include "../node.h"


class DirectionLightComponent : public Component
{
public:
    DirectionLightComponent();
    ~DirectionLightComponent();

    inline const vec3& getColor() const { return m_color; }
    inline void setColor(const vec3& color) { m_color[0] = color[0]; m_color[1] = color[1]; m_color[2] = color[2]; m_bLightDataDirty = true; }

    inline float getIntensity() const { return m_intensity; }
    inline void setIntensity(float intensity) { m_intensity = intensity; m_bLightDataDirty = true; }


    inline void updateLightData(DirectionLightGPUData* pOutLightData)
    {
        vec3 vecForward;
        getNode()->getRotationQuaternion().getForwardVector(vecForward);
        pOutLightData->m_vecDirection[0] = vecForward[0];
        pOutLightData->m_vecDirection[1] = vecForward[1];
        pOutLightData->m_vecDirection[2] = vecForward[2];

        pOutLightData->m_vecColor[0] = m_color[0] * m_intensity;
        pOutLightData->m_vecColor[1] = m_color[1] * m_intensity;
        pOutLightData->m_vecColor[2] = m_color[2] * m_intensity;

        m_bLightDataDirty = false;
    }

    bool isLightDataDirty() const { return m_bLightDataDirty; }
    void markLightDataDirty() { m_bLightDataDirty = true; }

    void onAddToNode() override;
    // void onStart() override {}
    void draw() override {}
    void update(float deltaTime) override {}

    bool isIDrawable() const override { return false; }
    bool isUpdatable() const override { return false; }


private:
    vec3 m_color;
    float m_intensity;

    bool m_bLightDataDirty = true;
};
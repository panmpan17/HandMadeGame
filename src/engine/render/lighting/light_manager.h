#pragma once

#include "light_data.h"

#include <linmath.h>
#include "../../../utils/expandable_array.h"

typedef unsigned int GLuint;
class PointLightComponent;
class DirectionLightComponent;


constexpr int MAX_DIRECTION_LIGHTS = 4;
constexpr int MAX_POINT_LIGHTS = 8;


class LightManager
{
public:
    inline static LightManager* getInstance() { return ins; }

    static void Initialize();
    static void Cleanup() { delete ins; ins = nullptr; }

    inline GLuint getLightingUBO() const { return m_nLightingUBO; }

    void updateLightingUBO();

    inline void registerPointLightComponent(PointLightComponent* pPointLightComp)
    {
        m_arrPointLightsComponents.addElement(pPointLightComp);
        ++m_nNumPointLights;
    }

    inline void unregisterPointLightComponent(PointLightComponent* pPointLightComp)
    {
        int nIndex = m_arrPointLightsComponents.getElementIndex(pPointLightComp);
        if (nIndex != -1)
        {
            m_arrPointLightsComponents.removeElement(nIndex);
            m_bUBODirty = true;
            --m_nNumPointLights;
        }
    }

    inline void registerDirectionLightComponent(DirectionLightComponent* pDirectionLightComp)
    {
        m_arrDirectionLightsComponents.addElement(pDirectionLightComp);
        ++m_nNumDirectionLights;
    }

    inline void unregisterDirectionLightComponent(DirectionLightComponent* pDirectionLightComp)
    {
        int nIndex = m_arrDirectionLightsComponents.getElementIndex(pDirectionLightComp);
        if (nIndex != -1)
        {
            m_arrDirectionLightsComponents.removeElement(nIndex);
            m_bUBODirty = true;
            --m_nNumDirectionLights;
        }
    }

private:
    static LightManager* ins;

    LightManager();
    ~LightManager();

    bool m_bUBODirty = true;
    GLuint m_nLightingUBO = 0;

    vec3 m_colorAmbientLight = {0.2f, 0.2f, 0.2f};

    DirectionLightGPUData m_vecDirectionLights[MAX_DIRECTION_LIGHTS];
    int m_nNumDirectionLights = 0;
    PointerExpandableArray<DirectionLightComponent*> m_arrDirectionLightsComponents{ MAX_DIRECTION_LIGHTS };
    
    PointLightGPUData m_vecPointLights[MAX_POINT_LIGHTS];
    int m_nNumPointLights = 0;
    PointerExpandableArray<PointLightComponent*> m_arrPointLightsComponents{ MAX_POINT_LIGHTS };
};
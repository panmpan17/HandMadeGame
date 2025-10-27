#pragma once

#include <linmath.h>

typedef unsigned int GLuint;


struct DirectionLight
{
    vec4 m_vecDirection; // vec3 + padding
    vec4 m_vecColor; // vec3 + padding
};

struct PointLight
{
    vec4 m_vecPositionAndRange;
    vec4 m_vecColor; // vec3 + padding
    vec4 m_vecAttenuationParams; // vec3 + padding
};



class LightManager
{
public:
    inline static LightManager* getInstance() { return ins; }

    static void Initialize();
    static void Cleanup() { delete ins; ins = nullptr; }

    GLuint getLightingUBO() const { return m_nLightingUBO; }

    void updateLightingUBO();

private:
    static LightManager* ins;

    LightManager();
    ~LightManager();

    bool m_bUBODirty = true;
    GLuint m_nLightingUBO = 0;

    vec3 m_colorAmbientLight = {0.2f, 0.2f, 0.2f};

    DirectionLight m_vecSunLights[4];
    int m_nNumSunLights = 0;

    PointLight m_vecPointLights[8];
    int m_nNumPointLights = 0;
};
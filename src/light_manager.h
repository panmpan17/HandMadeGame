#pragma once

#include <linmath.h>

typedef unsigned int GLuint;


class LightManager
{
public:
    inline static LightManager* getInstance() { return ins; }

    static void Initialize();
    static void Cleanup() { delete ins; ins = nullptr; }

    GLuint getLightingUBO() const { return m_nLightingUBO; }

private:
    static LightManager* ins;

    LightManager();
    ~LightManager();

    GLuint m_nLightingUBO = 0;

    vec3 m_colorAmbientLight = {1.2f, 0.2f, 0.2f};
};
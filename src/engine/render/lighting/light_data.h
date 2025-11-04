#pragma once

#include <linmath.h>

struct DirectionLightGPUData
{
    vec4 m_vecDirection; // vec3 + padding
    vec4 m_vecColor; // vec3 + padding
};

struct PointLightGPUData
{
    vec4 m_vecPositionAndRange;
    vec4 m_vecColor; // vec3 + padding
    vec4 m_vecAttenuationParams; // vec3 + padding
};
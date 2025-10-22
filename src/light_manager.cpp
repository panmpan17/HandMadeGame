#include "light_manager.h"
#include <glad/gl.h>


constexpr int MAX_SUN_LIGHTS = 4;
constexpr int DIRECTION_LIGHT_SIZE = 32; // vec3 + padding + vec3 + padding


LightManager* LightManager::ins = nullptr;

void LightManager::Initialize()
{
    if (!ins)
    {
        ins = new LightManager();
    }
}

LightManager::LightManager()
{
    m_vecSunLights[0].m_vecDirection[0] = 1.f;
    m_vecSunLights[0].m_vecDirection[1] = 0.f;
    m_vecSunLights[0].m_vecDirection[2] = 0.f;
    m_vecSunLights[0].m_vecColor[0] = 1.f;
    m_vecSunLights[0].m_vecColor[1] = 0.f;
    m_vecSunLights[0].m_vecColor[2] = 0.f;


    m_vecSunLights[1].m_vecDirection[0] = 0.f;
    m_vecSunLights[1].m_vecDirection[1] = 0.f;
    m_vecSunLights[1].m_vecDirection[2] = 1.f;
    m_vecSunLights[1].m_vecColor[0] = 0.f;
    m_vecSunLights[1].m_vecColor[1] = 0.f;
    m_vecSunLights[1].m_vecColor[2] = 1.f;


    m_vecSunLights[2].m_vecDirection[0] = 0.f;
    m_vecSunLights[2].m_vecDirection[1] = 1.f;
    m_vecSunLights[2].m_vecDirection[2] = 0.f;
    m_vecSunLights[2].m_vecColor[0] = 0.f;
    m_vecSunLights[2].m_vecColor[1] = 1.f;
    m_vecSunLights[2].m_vecColor[2] = 0.f;

    m_nNumSunLights = 3;

    glGenBuffers(1, &m_nLightingUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_nLightingUBO);
    glBufferData(GL_UNIFORM_BUFFER, 160, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

LightManager::~LightManager()
{
    glDeleteBuffers(1, &m_nLightingUBO);
}

void LightManager::updateLightingUBO()
{
    if (m_bUBODirty)
    {
        m_bUBODirty = false;

        glBindBuffer(GL_UNIFORM_BUFFER, m_nLightingUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec3), &m_colorAmbientLight);

        for (int i = 0; i < m_nNumSunLights; ++i)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4) + i * DIRECTION_LIGHT_SIZE, sizeof(vec3), &m_vecSunLights[i].m_vecDirection);
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4) + i * DIRECTION_LIGHT_SIZE + sizeof(vec4), sizeof(vec3), &m_vecSunLights[i].m_vecColor);
        }

        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4) + (MAX_SUN_LIGHTS * DIRECTION_LIGHT_SIZE), sizeof(int), &m_nNumSunLights);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}


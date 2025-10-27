#include "light_manager.h"
#include <glad/gl.h>

constexpr int AMBIENT_LIGHT_SIZE = 16; // vec3 + padding
constexpr int MAX_SUN_LIGHTS = 4;
constexpr int DIRECTION_LIGHT_SIZE = 32; // vec3 + padding + vec3 + padding
constexpr int MAX_POINT_LIGHTS = 8;
constexpr int POINT_LIGHT_SIZE = 32; // vec4 + vec3 + padding

constexpr int NUM_SUN_LIGHTS_SIZE = 16; // vec2 + padding

constexpr int LIGHTING_UBO_SIZE = AMBIENT_LIGHT_SIZE + (MAX_SUN_LIGHTS * DIRECTION_LIGHT_SIZE) + (MAX_POINT_LIGHTS * POINT_LIGHT_SIZE) + NUM_SUN_LIGHTS_SIZE; // Calculated total size


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


    m_vecSunLights[1].m_vecDirection[0] = -1.f;
    m_vecSunLights[1].m_vecDirection[1] = 0.f;
    m_vecSunLights[1].m_vecDirection[2] = 0.f;
    m_vecSunLights[1].m_vecColor[0] = 0.f;
    m_vecSunLights[1].m_vecColor[1] = 0.f;
    m_vecSunLights[1].m_vecColor[2] = 1.f;


    m_vecSunLights[2].m_vecDirection[0] = 0.f;
    m_vecSunLights[2].m_vecDirection[1] = -1.f;
    m_vecSunLights[2].m_vecDirection[2] = 0.f;
    m_vecSunLights[2].m_vecColor[0] = 0.f;
    m_vecSunLights[2].m_vecColor[1] = 1.f;
    m_vecSunLights[2].m_vecColor[2] = 0.f;

    m_nNumSunLights = 0;

    m_vecPointLights[0].m_vecPositionAndRange[0] = 0.f;
    m_vecPointLights[0].m_vecPositionAndRange[1] = 0.f;
    m_vecPointLights[0].m_vecPositionAndRange[2] = 0.f;
    m_vecPointLights[0].m_vecPositionAndRange[3] = 1.f;
    m_vecPointLights[0].m_vecColor[0] = 0.f;
    m_vecPointLights[0].m_vecColor[1] = 0.f;
    m_vecPointLights[0].m_vecColor[2] = 1.f;

    m_vecPointLights[1].m_vecPositionAndRange[0] = 5.f;
    m_vecPointLights[1].m_vecPositionAndRange[1] = 0.f;
    m_vecPointLights[1].m_vecPositionAndRange[2] = 0.f;
    m_vecPointLights[1].m_vecPositionAndRange[3] = 5.f;
    m_vecPointLights[1].m_vecColor[0] = 0.f;
    m_vecPointLights[1].m_vecColor[1] = 1.f;
    m_vecPointLights[1].m_vecColor[2] = 0.f;


    m_vecPointLights[2].m_vecPositionAndRange[0] = -5.f;
    m_vecPointLights[2].m_vecPositionAndRange[1] = 0.f;
    m_vecPointLights[2].m_vecPositionAndRange[2] = 0.f;
    m_vecPointLights[2].m_vecPositionAndRange[3] = 5.f;
    m_vecPointLights[2].m_vecColor[0] = 0.f;
    m_vecPointLights[2].m_vecColor[1] = 0.f;
    m_vecPointLights[2].m_vecColor[2] = 1.f;

    m_nNumPointLights = 2;

    glGenBuffers(1, &m_nLightingUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_nLightingUBO);
    glBufferData(GL_UNIFORM_BUFFER, LIGHTING_UBO_SIZE, nullptr, GL_STATIC_DRAW);
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
        unsigned long nOffset = 0;

        glBufferSubData(GL_UNIFORM_BUFFER, nOffset, sizeof(vec3), &m_colorAmbientLight);
        nOffset += sizeof(vec4); // vec3 + padding

        glBufferSubData(GL_UNIFORM_BUFFER, nOffset, MAX_SUN_LIGHTS * DIRECTION_LIGHT_SIZE, &m_vecSunLights); 
        nOffset += (MAX_SUN_LIGHTS * DIRECTION_LIGHT_SIZE);

        glBufferSubData(GL_UNIFORM_BUFFER, nOffset, MAX_POINT_LIGHTS * POINT_LIGHT_SIZE, &m_vecPointLights);
        nOffset += (MAX_POINT_LIGHTS * POINT_LIGHT_SIZE);

        glBufferSubData(GL_UNIFORM_BUFFER, nOffset, sizeof(int), &m_nNumSunLights);
        glBufferSubData(GL_UNIFORM_BUFFER, nOffset + sizeof(int), sizeof(int), &m_nNumPointLights);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}


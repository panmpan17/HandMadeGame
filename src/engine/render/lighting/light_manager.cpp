#include "light_manager.h"
#include <glad/gl.h>

#include "point_light.h"
#include "direction_light.h"
#include "../debug_macro.h"


constexpr int AMBIENT_LIGHT_SIZE = 16; // vec3 + padding
constexpr int DIRECTION_LIGHT_SIZE = 32; // vec3 + padding + vec3 + padding
constexpr int POINT_LIGHT_SIZE = 48; // vec4 + (vec3 + padding) + (vec3 + padding)

constexpr int NUM_SUN_LIGHTS_SIZE = 16; // vec2 + padding

constexpr int LIGHTING_UBO_SIZE = AMBIENT_LIGHT_SIZE + (MAX_DIRECTION_LIGHTS * DIRECTION_LIGHT_SIZE) + (MAX_POINT_LIGHTS * POINT_LIGHT_SIZE) + NUM_SUN_LIGHTS_SIZE; // Calculated total size


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
    int nNumDirectionLights = std::min(m_nNumDirectionLights, MAX_DIRECTION_LIGHTS);
    int nNumPointLights = std::min(m_nNumPointLights, MAX_POINT_LIGHTS);

    for (int i = 0; i < nNumDirectionLights; ++i)
    {
        DirectionLightComponent* const pDirectionLightComp = m_arrDirectionLightsComponents.getElement(i);
        if (pDirectionLightComp && pDirectionLightComp->isLightDataDirty())
        {
            m_bUBODirty = true;
            pDirectionLightComp->updateLightData(&m_vecDirectionLights[i]);
        }
    }

    for (int i = 0; i < nNumPointLights; ++i)
    {
        PointLightComponent* const pPointLightComp = m_arrPointLightsComponents.getElement(i);
        if (pPointLightComp && pPointLightComp->isLightDataDirty())
        {
            m_bUBODirty = true;
            pPointLightComp->updateLightData(&m_vecPointLights[i]);
        }
    }

    if (m_bUBODirty)
    {
        m_bUBODirty = false;

        glBindBuffer(GL_UNIFORM_BUFFER, m_nLightingUBO);
        unsigned long nOffset = 0;

        glBufferSubData(GL_UNIFORM_BUFFER, nOffset, sizeof(vec3), &m_colorAmbientLight);
        nOffset += sizeof(vec4); // vec3 + padding

        glBufferSubData(GL_UNIFORM_BUFFER, nOffset, MAX_DIRECTION_LIGHTS * DIRECTION_LIGHT_SIZE, &m_vecDirectionLights); 
        nOffset += (MAX_DIRECTION_LIGHTS * DIRECTION_LIGHT_SIZE);

        glBufferSubData(GL_UNIFORM_BUFFER, nOffset, MAX_POINT_LIGHTS * POINT_LIGHT_SIZE, &m_vecPointLights);
        nOffset += (MAX_POINT_LIGHTS * POINT_LIGHT_SIZE);

        glBufferSubData(GL_UNIFORM_BUFFER, nOffset, sizeof(int), &m_nNumDirectionLights);
        glBufferSubData(GL_UNIFORM_BUFFER, nOffset + sizeof(int), sizeof(int), &nNumPointLights);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}


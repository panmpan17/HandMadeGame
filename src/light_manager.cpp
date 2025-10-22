#include "light_manager.h"
#include <glad/gl.h>


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
    glBufferData(GL_UNIFORM_BUFFER, sizeof(vec3), &m_colorAmbientLight, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

LightManager::~LightManager()
{
    glDeleteBuffers(1, &m_nLightingUBO);
}


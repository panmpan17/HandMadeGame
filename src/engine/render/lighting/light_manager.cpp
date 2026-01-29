#include "light_manager.h"
#include <glad/gl.h>

#include "point_light.h"
#include "direction_light.h"
#include "../../core/window.h"


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
    registerLightingUBO();
    registerShadowDepthMap();
}

LightManager::~LightManager()
{
    if (Window::ins->isUsingOpenGL())
    {
        glDeleteBuffers(1, &m_nLightingUBO);

        glDeleteFramebuffers(1, &m_nShadowDepthMapFBO);
        glDeleteTextures(1, &m_nShadowDepthMapTexture);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        m_pLightingBuffer->release();
    }
#endif // __APPLE__
}

void LightManager::registerLightingUBO()
{
    if (Window::ins->isUsingOpenGL())
    {
        glGenBuffers(1, &m_nLightingUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, m_nLightingUBO);
        glBufferData(GL_UNIFORM_BUFFER, LIGHTING_UBO_SIZE, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        m_pLightingBuffer = Window::ins->getMetalDevice()->newBuffer(LIGHTING_UBO_SIZE, MTL::ResourceStorageModeShared);
    }
#endif // __APPLE__
}

void LightManager::registerShadowDepthMap()
{
    if (Window::ins->isUsingOpenGL())
    {
        glGenFramebuffers(1, &m_nShadowDepthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_nShadowDepthMapFBO);

        glGenTextures(1, &m_nShadowDepthMapTexture);
        glBindTexture(GL_TEXTURE_2D, m_nShadowDepthMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_nShadowDepthMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        MTL::TextureDescriptor* pTextureDesc = MTL::TextureDescriptor::alloc()->init();
        pTextureDesc->setPixelFormat(MTL::PixelFormatDepth32Float);
        pTextureDesc->setWidth(SHADOW_MAP_WIDTH);
        pTextureDesc->setHeight(SHADOW_MAP_HEIGHT);
        pTextureDesc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        pTextureDesc->setStorageMode(MTL::StorageModePrivate);

        m_pShadowDepthMapTextureMetal = Window::ins->getMetalDevice()->newTexture(pTextureDesc);

        pTextureDesc->release();
    }
#endif // __APPLE__
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

        if (Window::ins->isUsingOpenGL())
        {
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
#if __APPLE__
        else if (Window::ins->isUsingMetal())
        {
            void* pBufferContents = m_pLightingBuffer->contents();
            unsigned long nOffset = 0;

            memcpy((char*)pBufferContents + nOffset, &m_colorAmbientLight, sizeof(vec3));
            nOffset += sizeof(vec4); // vec3 + padding

            memcpy((char*)pBufferContents + nOffset, &m_vecDirectionLights, MAX_DIRECTION_LIGHTS * DIRECTION_LIGHT_SIZE);
            nOffset += (MAX_DIRECTION_LIGHTS * DIRECTION_LIGHT_SIZE);

            memcpy((char*)pBufferContents + nOffset, &m_vecPointLights, MAX_POINT_LIGHTS * POINT_LIGHT_SIZE);
            nOffset += (MAX_POINT_LIGHTS * POINT_LIGHT_SIZE);

            memcpy((char*)pBufferContents + nOffset, &m_nNumDirectionLights, sizeof(int));
            memcpy((char*)pBufferContents + nOffset + sizeof(int), &nNumPointLights, sizeof(int));

            m_pLightingBuffer->didModifyRange(NS::Range(0, LIGHTING_UBO_SIZE));
        }
#endif // __APPLE__
    }
}


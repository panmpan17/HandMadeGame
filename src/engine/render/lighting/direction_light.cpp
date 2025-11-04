#include "direction_light.h"

#include "light_manager.h"


DirectionLightComponent::DirectionLightComponent()
{
    LightManager::getInstance()->registerDirectionLightComponent(this);
}

DirectionLightComponent::~DirectionLightComponent()
{
    LightManager::getInstance()->unregisterDirectionLightComponent(this);
}

void DirectionLightComponent::onAddToNode()
{
    if (!m_pNode)
    {
        return;
    }

    m_pNode->registerOnPositionChangedListener(std::bind(&DirectionLightComponent::markLightDataDirty, this));
}


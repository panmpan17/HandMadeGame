#include "point_light.h"

#include "light_manager.h"


PointLightComponent::PointLightComponent()
{
    LightManager::getInstance()->registerPointLightComponent(this);
}

PointLightComponent::~PointLightComponent()
{
    LightManager::getInstance()->unregisterPointLightComponent(this);
}

void PointLightComponent::onAddToNode()
{
    if (!m_pNode)
    {
        return;
    }

    m_pNode->registerOnPositionChangedListener(std::bind(&PointLightComponent::markLightDataDirty, this));
}


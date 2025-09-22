#pragma once

#include "component.h"
#include "../node.h"
#include "../serialization/type_registry.h"


class Rotate : public Component
{
public:
    Rotate(float rotationSpeed = 0) : m_fRotationSpeed(rotationSpeed) {}

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float deltaTime) override
    {
        Node* pNode = getNode();
        if (pNode)
        {
            float newRotation = pNode->getRotation() + m_fRotationSpeed * deltaTime;
            pNode->setRotation(newRotation);
        }
    }

    virtual void draw() override {}

protected:
    float m_fRotationSpeed = 0.0f; // Speed of rotation in degrees per second

    COMPONENT_REGISTER_SERIALIZABLE(Rotate)
};

REGISTER_CLASS(Rotate)


#pragma once

#include "component.h"
#include "../node.h"


class Rotate : public IUpdatable
{
public:
    Rotate(float rotationSpeed) : m_fRotationSpeed(rotationSpeed) {}

    virtual void update(float deltaTime) override
    {
        Node* pNode = getNode();
        if (pNode)
        {
            float newRotation = pNode->getRotation() + m_fRotationSpeed * deltaTime;
            pNode->setRotation(newRotation);
        }
    }

private:
    float m_fRotationSpeed = 0.0f; // Speed of rotation in degrees per second
};


#pragma once

#include "component.h"
#include "../node.h"
#include "../serialization/type_registry.h"


// class Rotate : public Component
// {
// public:
//     Rotate(float rotationSpeed = 0) : m_fRotationSpeed(rotationSpeed) {}

//     virtual bool isIDrawable() const override { return false; }
//     virtual bool isUpdatable() const override { return true; }

//     virtual void update(float deltaTime) override
//     {
//         Node* pNode = getNode();
//         if (pNode)
//         {
//             float newRotation = pNode->getRotation() + m_fRotationSpeed * deltaTime;
//             pNode->setRotation(newRotation);
//         }
//     }

//     virtual void draw() override {}

// protected:
//     float m_fRotationSpeed = 0.0f; // Speed of rotation in degrees per second

//     COMPONENT_REGISTER_SERIALIZABLE(Rotate)
// };

// REGISTER_CLASS(Rotate)



class Rotate3D : public Component
{
public:
    Rotate3D(float fXSpeed = 0, float fYSpeed = 0, float fZSpeed = 0)
        : m_fXSpeed(fXSpeed), m_fYSpeed(fYSpeed), m_fZSpeed(fZSpeed) {}

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float deltaTime) override
    {
        Node* pNode = getNode();
        if (pNode)
        {
            pNode->rotateQuaternion(Quaternion::fromAxisAngle({ m_fXSpeed, m_fYSpeed, m_fZSpeed }, deltaTime));
        }
    }

    virtual void draw() override {}

protected:
    float m_fXSpeed = 0.0f; // Speed of rotation around the X-axis in degrees per second
    float m_fYSpeed = 0.0f; // Speed of rotation around the Y-axis in degrees per second
    float m_fZSpeed = 0.0f; // Speed of rotation around the Z-axis in degrees per second

    // COMPONENT_REGISTER_SERIALIZABLE(Rotate)
};

// REGISTER_CLASS(Rotate)


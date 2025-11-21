#pragma once

#include "../component.h"
#include "../../core/scene/node.h"
#include "../../core/serialization/type_registry.h"
#include "../../core/math/vector.h"


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
        : m_vecCurrentEulerAngles(fXSpeed, fYSpeed, fZSpeed) {}

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float deltaTime) override
    {
        Node* pNode = getNode();
        if (pNode)
        {
            pNode->rotateQuaternion(Quaternion::fromAxisAngle({ m_vecCurrentEulerAngles.x, m_vecCurrentEulerAngles.y, m_vecCurrentEulerAngles.z }, deltaTime));
        }
    }

    virtual void draw() override {}

protected:
    Vector3 m_vecCurrentEulerAngles = { 0.0f, 0.0f, 0.0f };

    COMPONENT_REGISTER_SERIALIZABLE(Rotate3D)
};

REGISTER_CLASS(Rotate3D)

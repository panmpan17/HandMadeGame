#include "first_person_free_control_camera.h"

#include "../node.h"
#include "../input_handle.h"
#include "../camera.h"

#define BIND_CALLBACK_1(func) std::bind(&FirstPersonFreeControlCamera::func, this, std::placeholders::_1)
#define BIND_CALLBACK_2(func) std::bind(&FirstPersonFreeControlCamera::func, this, std::placeholders::_1, std::placeholders::_2)


FirstPersonFreeControlCamera::FirstPersonFreeControlCamera()
{
    InputManager* pInput = InputManager::getInstance();

    // pInput->registerKeyPressCallback(KeyCode::KEY_W, BIND_CALLBACK_1(onKeyboardW));
    // pInput->registerKeyPressCallback(KeyCode::KEY_S, BIND_CALLBACK_1(onKeyboardS));
    // pInput->registerKeyPressCallback(KeyCode::KEY_A, BIND_CALLBACK_1(onKeyboardA));
    // pInput->registerKeyPressCallback(KeyCode::KEY_D, BIND_CALLBACK_1(onKeyboardD));
    pInput->registerMouseMoveCallback(BIND_CALLBACK_2(onMouseMove));
    pInput->registerKeyPressCallback(KeyCode::KEY_R, BIND_CALLBACK_1(onResetRotationKeyDown));
    pInput->registerKeyPressCallback(KeyCode::KEY_LEFT_SHIFT, BIND_CALLBACK_1(onSpeedModifierKeyDown));

}

FirstPersonFreeControlCamera::~FirstPersonFreeControlCamera()
{

}

void FirstPersonFreeControlCamera::update(float fDeltaTime)
{
    if (!m_pNode)
    {
        return;
    }

    InputManager* pInput = InputManager::getInstance();

    if (!pInput->isMouseButtonPressed(MouseButton::BUTTON_RIGHT))
    {
        return;
    }

    int fHorizontal = (pInput->isKeyPressed(KeyCode::KEY_D) ? 1 : 0) + (pInput->isKeyPressed(KeyCode::KEY_A) ? -1 : 0);
    int fVertical = (pInput->isKeyPressed(KeyCode::KEY_E) ? 1 : 0) + (pInput->isKeyPressed(KeyCode::KEY_Q) ? -1 : 0);
    int fForward = (pInput->isKeyPressed(KeyCode::KEY_W) ? 1 : 0) + (pInput->isKeyPressed(KeyCode::KEY_S) ? -1 : 0);

    if (fHorizontal != 0 || fVertical != 0 || fForward != 0)
    {
        Vector3 vecPos = m_pNode->getPosition();

        vec3 vecForward, vecUp, vecRight;
        m_pNode->getRotationQuaternion().getForwardVector(vecForward);
        m_pNode->getRotationQuaternion().getUpVector(vecUp);
        m_pNode->getRotationQuaternion().getRightVector(vecRight);

        vecPos.x += (vecForward[0] * fForward + vecRight[0] * fHorizontal + vecUp[0] * fVertical) * m_fMoveSpeed * fDeltaTime;
        vecPos.y += (vecForward[1] * fForward + vecRight[1] * fHorizontal + vecUp[1] * fVertical) * m_fMoveSpeed * fDeltaTime;
        vecPos.z += (vecForward[2] * fForward + vecRight[2] * fHorizontal + vecUp[2] * fVertical) * m_fMoveSpeed * fDeltaTime;

        m_pNode->setPosition(vecPos);
    }

    if (m_fMouseDeltaX != 0.0f || m_fMouseDeltaY != 0.0f)
    {
        m_pNode->rotateQuaternion(Quaternion::fromAxisAngle({m_fMouseDeltaY, m_fMouseDeltaX, 0}, m_fRotationSpeed * fDeltaTime));

        m_fMouseDeltaX = 0.0f;
        m_fMouseDeltaY = 0.0f;
    }
}

void FirstPersonFreeControlCamera::onMouseMove(float fDeltaX, float fDeltaY)
{
    m_fMouseDeltaX = fDeltaX;
    m_fMouseDeltaY = fDeltaY;
}

void FirstPersonFreeControlCamera::onResetRotationKeyDown(bool bPressed)
{
    if (bPressed && InputManager::getInstance()->isMouseButtonPressed(MouseButton::BUTTON_RIGHT))
    {
        if (m_pNode)
        {
            m_pNode->setRotationQuaternion(Quaternion());
        }
    }
}

void FirstPersonFreeControlCamera::onSpeedModifierKeyDown(bool bPressed)
{
    if (bPressed)
    {
        m_fMoveSpeed = BOOSTED_MOVE_SPEED;
    }
    else
    {
        m_fMoveSpeed = DEFAULT_MOVE_SPEED;
    }
}

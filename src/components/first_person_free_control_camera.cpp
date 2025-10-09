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


    float fMoveSpeed = 5.0f; // units per second

    int fHorizontal = (pInput->isKeyPressed(KeyCode::KEY_D) ? 1 : 0) + (pInput->isKeyPressed(KeyCode::KEY_A) ? -1 : 0);
    int fVertical = (pInput->isKeyPressed(KeyCode::KEY_W) ? 1 : 0) + (pInput->isKeyPressed(KeyCode::KEY_S) ? -1 : 0);

    if (fHorizontal != 0 || fVertical != 0)
    {
        m_pNode->move(fHorizontal * fMoveSpeed * fDeltaTime, fVertical * fMoveSpeed * fDeltaTime);
    }

    if (m_fMouseDeltaX != 0.0f || m_fMouseDeltaY != 0.0f)
    {
        m_pNode->rotateQuaternion(Quaternion::fromAxisAngle({m_fMouseDeltaX, m_fMouseDeltaY, 0}, fDeltaTime));

        m_fMouseDeltaX = 0.0f;
        m_fMouseDeltaY = 0.0f;
    }
    
    updateCameraPositionToNode();
}

void FirstPersonFreeControlCamera::onMouseMove(float fDeltaX, float fDeltaY)
{
    m_fMouseDeltaX = fDeltaX;
    m_fMouseDeltaY = fDeltaY;
}

void FirstPersonFreeControlCamera::updateCameraPositionToNode()
{
    if (!m_pNode)
    {
        return;
    }

    const Vector3& vecPosition = m_pNode->getPosition();
    Camera::main->setPosition(vecPosition);

    vec3 vecForward;
    m_pNode->getRotationQuaternion().getForwardVector(vecForward);

    vecForward[0] += vecPosition.x;
    vecForward[1] += vecPosition.y;
    vecForward[2] += vecPosition.z;
    Camera::main->setPointAt(vecForward[0], vecForward[1], vecForward[2]);
}

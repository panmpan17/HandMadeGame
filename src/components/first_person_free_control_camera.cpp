#include "first_person_free_control_camera.h"

#include "../node.h"
#include "../input_handle.h"
#include "../camera.h"

#define BIND_CALLBACK_1(func) std::bind(&FirstPersonFreeControlCamera::func, this, std::placeholders::_1)


FirstPersonFreeControlCamera::FirstPersonFreeControlCamera()
{
    InputManager* pInput = InputManager::getInstance();

    // pInput->registerKeyPressCallback(KeyCode::KEY_W, BIND_CALLBACK_1(onKeyboardW));
    // pInput->registerKeyPressCallback(KeyCode::KEY_S, BIND_CALLBACK_1(onKeyboardS));
    // pInput->registerKeyPressCallback(KeyCode::KEY_A, BIND_CALLBACK_1(onKeyboardA));
    // pInput->registerKeyPressCallback(KeyCode::KEY_D, BIND_CALLBACK_1(onKeyboardD));


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

    vec3 vecPosition, vecPointAt;
    Camera::main->getPosition(vecPosition);
    Camera::main->getPointAt(vecPointAt);

    if (pInput->isKeyPressed(KeyCode::KEY_W))
    {
        vecPosition[1] += fMoveSpeed * fDeltaTime;
        vecPointAt[1] += fMoveSpeed * fDeltaTime;
    }
    if (pInput->isKeyPressed(KeyCode::KEY_S))
    {
        vecPosition[1] -= fMoveSpeed * fDeltaTime;
        vecPointAt[1] -= fMoveSpeed * fDeltaTime;
    }
    if (pInput->isKeyPressed(KeyCode::KEY_A))
    {
        vecPosition[0] -= fMoveSpeed * fDeltaTime;
        vecPointAt[0] -= fMoveSpeed * fDeltaTime;
    }
    if (pInput->isKeyPressed(KeyCode::KEY_D))
    {
        vecPosition[0] += fMoveSpeed * fDeltaTime;
        vecPointAt[0] += fMoveSpeed * fDeltaTime;
    }

    Camera::main->setPosition(vecPosition[0], vecPosition[1], vecPosition[2]);
    Camera::main->setPointAt(vecPointAt[0], vecPointAt[1], vecPointAt[2]);
}

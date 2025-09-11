#include "paddle_control.h"
#include "../../input_handle.h"
#include "../../node.h"

#define BIND_CALLBACK_1(func) std::bind(&PaddleControl::func, this, std::placeholders::_1)


PaddleControl::PaddleControl(PaddleControlType eControlType, float fMaxSpeed) : m_eControlType(eControlType), m_fMaxSpeed(fMaxSpeed)
{
    switch (m_eControlType)
    {
    case PaddleControlType::PLAYER1:
        InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_W, BIND_CALLBACK_1(onUpPressCallback));
        InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_S, BIND_CALLBACK_1(onDownPressCallback));
        break;
    case PaddleControlType::PLAYER2:
        InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_O, BIND_CALLBACK_1(onUpPressCallback));
        InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_L, BIND_CALLBACK_1(onDownPressCallback));
        break;

    default:
        break;
    }
}

PaddleControl::~PaddleControl()
{
    // InputManager::getInstance()->unregisterKeyPressCallback(KeyCode::KEY_W);
    // InputManager::getInstance()->unregisterKeyPressCallback(KeyCode::KEY_S);
}

void PaddleControl::onUpPressCallback(bool bPressed)
{
    m_bUpPressed = bPressed;
}

void PaddleControl::onDownPressCallback(bool bPressed)
{
    m_bDownPressed = bPressed;
}

void PaddleControl::update(float deltaTime)
{
    if (!m_pNode) return;

    float fMoveDistance = m_fMaxSpeed * deltaTime;
    float posY = m_pNode->getPositionY();
    if (m_bUpPressed && !m_bDownPressed)
    {
        posY += fMoveDistance;

        if (posY > m_fPositionMaxY) posY = m_fPositionMaxY;
        m_pNode->setPosition(m_pNode->getPositionX(), posY);
    }
    else if (m_bDownPressed && !m_bUpPressed)
    {
        posY -= fMoveDistance;

        if (posY < m_fPositionMinY) posY = m_fPositionMinY;
        m_pNode->setPosition(m_pNode->getPositionX(), posY);
    }
}

#include "movement.h"
#include "../input_handle.h"


Movement::Movement(float fMoveSpeed)
    : m_fMoveSpeed(fMoveSpeed)
{
    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_W, [this](bool bPressed) {
        if (bPressed) {
            m_bMovementKeyPressed.set(0, true); // Set W key pressed
        } else {
            m_bMovementKeyPressed.set(0, false); // Reset W key pressed
        }
    });

    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_S, [this](bool bPressed) {
        if (bPressed) {
            m_bMovementKeyPressed.set(1, true); // Set S key pressed
        } else {
            m_bMovementKeyPressed.set(1, false); // Reset S key pressed
        }
    });

    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_A, [this](bool bPressed) {
        if (bPressed) {
            m_bMovementKeyPressed.set(2, true); // Set A key pressed
        } else {
            m_bMovementKeyPressed.set(2, false); // Reset A key pressed
        }
    });

    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_D, [this](bool bPressed) {
        if (bPressed) {
            m_bMovementKeyPressed.set(3, true); // Set D key pressed
        } else {
            m_bMovementKeyPressed.set(3, false); // Reset D key pressed
        }
    });
}

Movement::~Movement()
{
}


void Movement::update(float fDeltaTime)
{
    short x = 0, y = 0;
    getMovementDirection(x, y);

    if (x != 0 || y != 0) {
        // Assuming m_pNode is a pointer to the Node this component is attached to
        auto pNode = getNode();
        if (pNode) {
            pNode->move(x * m_fMoveSpeed * fDeltaTime, y * m_fMoveSpeed * fDeltaTime);
        }
    }
}

void Movement::getMovementDirection(short& x, short& y)
{
    x = m_bMovementKeyPressed.test(3) - m_bMovementKeyPressed.test(2);
    y = m_bMovementKeyPressed.test(0) - m_bMovementKeyPressed.test(1);
}

float lerp(float fStart, float fEnd, float fT)
{
    return fStart + (fEnd - fStart) * fT;
}

void TwoPointsMovement::update(float fDeltaTime)
{
    // Update the position based on the direction and speed
    auto pNode = getNode();
    if (!pNode) return;

    if (m_bIsMovingForward)
    {
        m_fTimer += fDeltaTime;
    }
    else
    {
        m_fTimer -= fDeltaTime;
    }

    if (m_fTimer >= m_fDuration) {
        m_fTimer = m_fDuration;
        m_bIsMovingForward = !m_bIsMovingForward;
    }
    if (m_fTimer <= 0) {
        m_fTimer = 0;
        m_bIsMovingForward = !m_bIsMovingForward;
    }

    float fProgress = m_fTimer / m_fDuration;
    float fX = lerp(m_vecStart[0], m_vecEnd[0], fProgress);
    float fY = lerp(m_vecStart[1], m_vecEnd[1], fProgress);

    pNode->setPosition(fX, fY);
}

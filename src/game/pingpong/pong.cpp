#include <linmath.h>
#include "pong.h"
#include "../../node.h"


Pong::Pong(float fSize, float fMaxSpeed) : m_fHalfSize(fSize / 2), m_fMaxSpeed(fMaxSpeed)
{
    m_vecDirection.normalize();
}

Pong::~Pong()
{
}

void Pong::update(float deltaTime)
{
    if (!m_pNode) return;

    Vector2 vecPos = m_pNode->getPosition();
    vecPos += m_vecDirection * (m_fMaxSpeed * deltaTime);

    if (vecPos.y > m_fPositionMaxY - m_fHalfSize)
    {
        vecPos.y = m_fPositionMaxY - m_fHalfSize;
        m_vecDirection.y = -m_vecDirection.y;
    }
    else if (vecPos.y < m_fPositionMinY + m_fHalfSize)
    {
        vecPos.y = m_fPositionMinY + m_fHalfSize;
        m_vecDirection.y = -m_vecDirection.y;
    }

    m_pNode->setPosition(vecPos);
}
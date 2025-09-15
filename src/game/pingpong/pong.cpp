#include <linmath.h>
#include <iostream>
#include "pong.h"
#include "paddle_control.h"
#include "../../node.h"
#include "../../components/quad.h"
#include "../../draw/shader_loader.h"
#include "../../physics/aabb.h"
#include "../../random.h"


Pong::Pong(const Box& oBox, PaddleControl* pPaddleControlLeft, PaddleControl* pPaddleControlRight, float fStartSpeed, float fMaxSpeed) :
    m_oBox(oBox), m_fHalfSize(oBox.getSizeX() / 2),
    m_fSpeed(fStartSpeed), m_fStartSpeed(fStartSpeed), m_fMaxSpeed(fMaxSpeed),
    m_pPaddleControlLeft(pPaddleControlLeft), m_pPaddleControlRight(pPaddleControlRight)
{
    m_vecDirection.normalize();
}

Pong::~Pong()
{
}

Box Pong::getBox() const 
{
    return m_oBox + m_pNode->getPosition();
}

void Pong::start()
{
    auto pQuad = new Quad(.6f, .6f, vec4 { 1.8f, 1.8f, 1.8f, 1.f });
    pQuad->setShader(ShaderLoader::getInstance()->getShader("image"));
    pQuad->registerBuffer();
    m_pNode->addComponent(pQuad);
}

void Pong::update(float fDeltaTime)
{
    switch (m_eState)
    {
    case PongState::SERVE:
        {
            m_vecDirection = Vector2(randomInt(0, 1) == 0 ? 1.0f : -1.0f, randomFloat(-0.5f, 0.5f));
            m_vecDirection.normalize();
            m_fSpeed = m_fStartSpeed;
            m_eState = PongState::PLAY;
        }
        break;
    case PongState::PLAY:
        updatePlaying(fDeltaTime);
        break;
    case PongState::SCORED:
        break;
    }
}

void Pong::updatePlaying(float fDeltaTime)
{
    Vector2 vecPos = m_pNode->getPosition();
    vecPos += m_vecDirection * (m_fSpeed * fDeltaTime);

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

    Box boxLeft = m_pPaddleControlLeft->getBox();
    Box boxRight = m_pPaddleControlRight->getBox();
    Box boxPong = getBox();

    if (AABB::isColliding(boxPong, boxLeft))
    {
        // vecPos.x = boxLeft.m_fEndX + m_fHalfSize;
        m_vecDirection.x = std::abs(m_vecDirection.x);

        m_fSpeed = std::min(m_fSpeed + m_fSpeedIncreasePerHit, m_fMaxSpeed);
    }
    else if (AABB::isColliding(boxPong, boxRight))
    {
        // vecPos.x = boxRight.m_fStartX - m_fHalfSize;
        m_vecDirection.x =  -std::abs(m_vecDirection.x);

        m_fSpeed = std::min(m_fSpeed + m_fSpeedIncreasePerHit, m_fMaxSpeed);
    }
    else if (vecPos.x < -5.0f || vecPos.x > 5.0f)
    {
        m_eState = PongState::SCORED;
        vecPos = Vector2(0, 0);
        m_eState = PongState::SERVE;
    }

    m_pNode->setPosition(vecPos);
}
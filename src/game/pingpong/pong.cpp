#include <linmath.h>
#include <iostream>
#include "pong.h"
#include "paddle_control.h"
#include "../../engine/render/shader_loader.h"
#include "../../engine/physics/aabb.h"
#include "../../engine/core/math/random.h"
#include "../../engine/core/scene/node.h"
#include "../../engine/components/render/quad.h"
#include "../../engine/components/particle/particle_system.h"
#include "../../engine/components/particle/particle_spawn.h"


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
    m_pQuad = new Quad(.6f, .6f, vec4 { 1.8f, 1.8f, 1.8f, 1.f });
    m_pQuad->setShader(ShaderLoader::getInstance()->getShader("image"));
    m_pQuad->registerBuffer();
    m_pNode->addComponent(m_pQuad);



    Shader* pParticleShader = ShaderLoader::getInstance()->getShader("particle_instance");

    m_pTestParticle = new ParticleSystem(20, false);

    m_pTestParticle->setShader(pParticleShader);
    m_pTestParticle->registerBuffer();
    m_pTestParticle->setParticleStartColor({ 1.f, 1.f, .3f, .2f }, { 1.f, 1.f, 0.3f, .4f });
    m_pTestParticle->setIsLooping(false);
    m_pTestParticle->addParticleModule(new ParticleBurstSpawn(0, 20));
    // m_pTestParticle->addParticleIndividualModule(new ScaleThroughParticleLifetime(0.1f, 1.f));
    m_pTestParticle->setParticleLifetime(.8f, 1);
    m_pTestParticle->setParticleStartVelocity(.8, 1);
    m_pTestParticle->setParticleStartScale(0.2f, 0.25f);
    m_pTestParticle->setGravity({ 0, -0.6f });
    m_pTestParticle->stop();

    m_pNode->addComponent(m_pTestParticle);
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

            float fProgress = ((m_fSpeed - m_fStartSpeed) / (m_fMaxSpeed - m_fStartSpeed));
            m_pQuad->setColor(
                m_vecStartColor[0],
                m_vecStartColor[1],
                m_vecStartColor[2],
                1.f
            );
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

        float fProgress = ((m_fSpeed - m_fStartSpeed) / (m_fMaxSpeed - m_fStartSpeed));
        m_pQuad->setColor(
            m_vecStartColor[0] + (m_vecEndColor[0] - m_vecStartColor[0]) * fProgress,
            m_vecStartColor[1] + (m_vecEndColor[1] - m_vecStartColor[1]) * fProgress,
            m_vecStartColor[2] + (m_vecEndColor[2] - m_vecStartColor[2]) * fProgress,
            1.f
        );

        m_pTestParticle->play(true);
    }
    else if (AABB::isColliding(boxPong, boxRight))
    {
        // vecPos.x = boxRight.m_fStartX - m_fHalfSize;
        m_vecDirection.x =  -std::abs(m_vecDirection.x);

        m_fSpeed = std::min(m_fSpeed + m_fSpeedIncreasePerHit, m_fMaxSpeed);

        float fProgress = ((m_fSpeed - m_fStartSpeed) / (m_fMaxSpeed - m_fStartSpeed));
        m_pQuad->setColor(
            m_vecStartColor[0] + (m_vecEndColor[0] - m_vecStartColor[0]) * fProgress,
            m_vecStartColor[1] + (m_vecEndColor[1] - m_vecStartColor[1]) * fProgress,
            m_vecStartColor[2] + (m_vecEndColor[2] - m_vecStartColor[2]) * fProgress,
            1.f
        );

        m_pTestParticle->play(true);
    }
    else if (vecPos.x < -5.0f || vecPos.x > 5.0f)
    {
        m_eState = PongState::SCORED;
        vecPos = Vector2(0, 0);
        m_eState = PongState::SERVE;
    }

    m_pNode->setPosition(vecPos);
}
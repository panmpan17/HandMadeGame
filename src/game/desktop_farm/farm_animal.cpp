#include "farm_animal.h"

#include "../../engine/core/math/general.h"
#include "../../engine/core/math/random.h"
#include "../../engine/components/render/sprite_animation.h"


FarmAnimal::FarmAnimal(SpriteAnimation* const pSpriteAnimation)
    : m_pSpriteAnimation(pSpriteAnimation)
{
}

FarmAnimal::~FarmAnimal()
{
}

void FarmAnimal::onStart()
{
    if (m_pSpriteAnimation)
    {
        m_nIdleAnimationIndex = m_pSpriteAnimation->getAnimationIndexByName("idle");
        m_nWalkAnimationIndex = m_pSpriteAnimation->getAnimationIndexByName("walk");
    }

    m_fIdleTimer = randomFloat(4, 7);
}

void FarmAnimal::update(float fDeltaTime)
{
    switch (m_eAnimateState)
    {
        case AnimalState::Idle:
            m_fIdleTimer -= fDeltaTime;
            if (m_fIdleTimer <= 0)
            {
                m_eAnimateState = AnimalState::Walking;

                const Vector3& oPosition = getNode()->getPosition();
                m_fDestinationX = oPosition.x + ((randomTwoChoice() ? 1 : -1) * randomFloat(2, 4));
                m_pSpriteAnimation->playAnimationInfo(m_nWalkAnimationIndex);

                Vector3 oSCale = getNode()->getScale();
                oSCale.x = std::abs(oSCale.x) * (m_fDestinationX > oPosition.x ? 1 : -1);
                getNode()->setScale(oSCale);
            }
            break;

        case AnimalState::Walking:
            Vector3 oPosition = getNode()->getPosition();

            bool bIsReached;
            oPosition.x = moveTowards(oPosition.x, m_fDestinationX, m_fWalkSpeed * fDeltaTime, bIsReached);
            getNode()->setPosition(oPosition);

            if (bIsReached)
            {
                m_eAnimateState = AnimalState::Idle;
                m_fIdleTimer = randomFloat(4, 7);
                m_pSpriteAnimation->playAnimationInfo(m_nIdleAnimationIndex);
            }
            break;
    }
}

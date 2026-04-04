#pragma once

#include "../../engine/components/component.h"

class SpriteAnimation;


enum class AnimalState
{
    Idle,
    Walking,
};


class FarmAnimal : public NodeComponent
{
public:
    FarmAnimal(SpriteAnimation* const pSpriteAnimation);
    ~FarmAnimal();

    virtual inline bool isIDrawable() const override { return false; }
    virtual inline bool isUpdatable() const override { return true; }

    virtual void onStart() override;
    virtual void update(float fDeltaTime) override;

private:
    SpriteAnimation* m_pSpriteAnimation = nullptr;

    AnimalState m_eAnimateState = AnimalState::Idle;
    float m_fIdleTimer = -1;
    float m_fDestinationX = 0;

    int m_nIdleAnimationIndex = -1;
    int m_nWalkAnimationIndex = -1;

    float m_fWalkSpeed = 1;
};

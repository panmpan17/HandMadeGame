#pragma once

#include "../../components/component.h"
#include "../../vector.h"
#include "../../physics/box.h"

class PaddleControl;


enum class PongState
{
    SERVE,
    PLAY,
    SCORED
};


class Pong : public Component
{
public:
    Pong(const Box& oBox, PaddleControl* pPaddleControlLeft, PaddleControl* pPaddleControlRight, float fStartSpeed, float fMaxSpeed);
    ~Pong();

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    inline void setPositionBounds(float fMinY, float fMaxY) { m_fPositionMinY = fMinY; m_fPositionMaxY = fMaxY; }

    void start();
    void update(float fDeltaTime) override;
    void draw() override {}

    Box getBox() const;

private:
    PongState m_eState = PongState::SERVE;

    Box m_oBox;
    float m_fHalfSize;
    float m_fSpeed;
    float m_fStartSpeed;
    float m_fMaxSpeed;
    float m_fSpeedIncreasePerHit = 0.1f;
    Vector2 m_vecDirection = Vector2(1.0f, 1.0f);

    float m_fPositionMaxY = 4.5f;
    float m_fPositionMinY = -4.5f;

    PaddleControl* m_pPaddleControlLeft;
    PaddleControl* m_pPaddleControlRight;

    void updatePlaying(float fDeltaTime);
};

#pragma once

#include <bitset>
#include <functional>
#include "component.h"
#include "../node.h"


class Movement : public Component
{
public:
    Movement(float fMoveSpeed = 0);
    virtual ~Movement();

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float fDeltaTime) override;

    virtual void draw() override {}

protected:
    float m_fMoveSpeed = 0.0f;
    std::bitset<4> m_bMovementKeyPressed = 0;

    void getMovementDirection(short& x, short& y);

    virtual void serializeToWrapper(DataSerializer& serializer) const override;
};


class TwoPointsMovement : public Component
{
public:
    TwoPointsMovement(const vec2& vecStart, const vec2& vecEnd, float duration)
        : m_fDuration(duration)
    {
        vec2_dup(m_vecStart, vecStart);
        vec2_dup(m_vecEnd, vecEnd);
    }

    virtual ~TwoPointsMovement() {}

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float fDeltaTime) override;

    virtual void draw() override {}

protected:
    vec2 m_vecStart;
    vec2 m_vecEnd;
    float m_fDuration;
    float m_fTimer = 0;

    // TODO: make this flexible
    bool m_bIsMovingForward = false;
};

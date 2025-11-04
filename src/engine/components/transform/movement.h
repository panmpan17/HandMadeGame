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

    COMPONENT_REGISTER_SERIALIZABLE(Movement)
};

REGISTER_CLASS(Movement)


class TwoPointsMovement : public Component
{
public:
    TwoPointsMovement()
    {
        m_vecStart[0] = 0;
        m_vecStart[1] = 0;
        m_vecEnd[0] = 0;
        m_vecEnd[1] = 0;
        m_fDuration = 1.0f;
    }
    TwoPointsMovement(const vec3& vecStart, const vec3& vecEnd, float duration)
        : m_fDuration(duration)
    {
        vec3_dup(m_vecStart, vecStart);
        vec3_dup(m_vecEnd, vecEnd);
    }

    virtual ~TwoPointsMovement() {}

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float fDeltaTime) override;

    virtual void draw() override {}

protected:
    vec3 m_vecStart;
    vec3 m_vecEnd;
    float m_fDuration;
    float m_fTimer = 0;

    // TODO: make this flexible
    bool m_bIsMovingForward = false;

    COMPONENT_REGISTER_SERIALIZABLE(TwoPointsMovement)
};

REGISTER_CLASS(TwoPointsMovement)

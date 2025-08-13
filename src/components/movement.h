#pragma once

#include <bitset>
#include "component.h"
#include "../node.h"


class Movement : public Component
{
public:
    Movement(float fMoveSpeed);
    virtual ~Movement();

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float fDeltaTime) override;

    virtual void draw() override {}

private:
    float m_fMoveSpeed = 0.0f;
    std::bitset<4> m_bMovementKeyPressed = 0;

    void getMovementDirection(short& x, short& y);
};


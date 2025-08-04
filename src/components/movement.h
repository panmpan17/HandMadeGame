#pragma once

#include <bitset>
#include "component.h"
#include "../node.h"


class Movement : public IUpdatable
{
public:
    Movement(float fMoveSpeed);
    virtual ~Movement();

    virtual void update(float fDeltaTime) override;

private:
    float m_fMoveSpeed = 0.0f;
    std::bitset<4> m_bMovementKeyPressed = 0;

    void getMovementDirection(short& x, short& y);
};


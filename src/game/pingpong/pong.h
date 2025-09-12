#pragma once

#include "../../components/component.h"
#include "../../vector.h"


class Pong : public Component
{
public:
    Pong(float fSize, float fMaxSpeed);
    ~Pong();

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    inline void setPositionBounds(float fMinY, float fMaxY) { m_fPositionMinY = fMinY; m_fPositionMaxY = fMaxY; }

    void update(float deltaTime) override;
    void draw() override {}

private:
    float m_fHalfSize;
    float m_fMaxSpeed;
    Vector2 m_vecDirection = Vector2(1.0f, 1.0f);

    float m_fPositionMaxY = 4.5f;
    float m_fPositionMinY = -4.5f;
};

#pragma once

#include "../../components/component.h"

enum class PaddleControlType
{
    PLAYER1,
    PLAYER2,
    AI
};

// class PaddleControl
class PaddleControl : public Component
{
public:
    PaddleControl(PaddleControlType eControlType, float fMaxSpeed = 5.0f);
    ~PaddleControl();

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    inline void setPositionBounds(float fMinY, float fMaxY) { m_fPositionMinY = fMinY; m_fPositionMaxY = fMaxY; }

    void update(float deltaTime) override;
    void draw() override {}

private:
    PaddleControlType m_eControlType;
    float m_fMaxSpeed;

    float m_fPositionMaxY = 4.5f;
    float m_fPositionMinY = -4.5f;

    bool m_bUpPressed = false;
    bool m_bDownPressed = false;

    void onUpPressCallback(bool bPressed);
    void onDownPressCallback(bool bPressed);
};

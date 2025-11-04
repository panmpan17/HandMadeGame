#include "component.h"


class FirstPersonFreeControlCamera : public Component
{
public:
    FirstPersonFreeControlCamera();
    ~FirstPersonFreeControlCamera();

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float fDeltaTime) override;

    virtual void draw() override {}

private:
    const float DEFAULT_MOVE_SPEED = 5.f;
    const float BOOSTED_MOVE_SPEED = 15.f;
    const float DEFAULT_ROTATION_SPEED = 15.f;

    float m_fMouseDeltaX = 0.0f;
    float m_fMouseDeltaY = 0.0f;

    float m_fMoveSpeed = DEFAULT_MOVE_SPEED;

    float m_fRotationSpeed = DEFAULT_ROTATION_SPEED;

    // void onRightMouseButton(bool bPressed);
    // void onKeyboardW(bool bPressed);
    // void onKeyboardS(bool bPressed);
    // void onKeyboardA(bool bPressed);
    // void onKeyboardD(bool bPressed);
    void onMouseMove(float fDeltaX, float fDeltaY);
    void onResetRotationKeyDown(bool bPressed);
    void onSpeedModifierKeyDown(bool bPressed);
};

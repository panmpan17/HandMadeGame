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
    float m_fMouseDeltaX = 0.0f;
    float m_fMouseDeltaY = 0.0f;

    float m_fMoveSpeed = 5.f;

    float m_fRotationSpeed = 15.f;

    // void onRightMouseButton(bool bPressed);
    // void onKeyboardW(bool bPressed);
    // void onKeyboardS(bool bPressed);
    // void onKeyboardA(bool bPressed);
    // void onKeyboardD(bool bPressed);
    void onMouseMove(float fDeltaX, float fDeltaY);
    void updateCameraPositionToNode();
};

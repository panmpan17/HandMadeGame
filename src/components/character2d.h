#include "sprite_animation.h"
#include "component.h"
#include "../input_handle.h"
#include "../debug_macro.h"

class Character2d : public Component
{
public:
    Character2d(SpriteAnimation* pAnimation) : m_pAnimation(pAnimation)
    {
        auto IdleAnimationIndex = new ushort[] {12};
        auto pIdleAnimation = new SpriteAnimationInfo(IdleAnimationIndex, 1, 3.f);
        m_nIdleAnimationIndex = m_pAnimation->addAnimationInfo(pIdleAnimation);

        auto nWalkAnimationIndex = new ushort[] {15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1};
        auto pWalkAnimation = new SpriteAnimationInfo(nWalkAnimationIndex, 11, 0.06f);
        m_nWalkAnimationIndex = m_pAnimation->addAnimationInfo(pWalkAnimation);

        pAnimation->playAnimationInfo(m_nIdleAnimationIndex);

        delete[] nWalkAnimationIndex;
        delete[] IdleAnimationIndex;


        InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_D, [this](bool bPressed) {
            if (bPressed) {
                m_pAnimation->playAnimationInfo(m_nWalkAnimationIndex);
            } else {
                m_pAnimation->playAnimationInfo(m_nIdleAnimationIndex);
            }
        });
    }


    ~Character2d()
    {
    }

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float fDeltaTime) override {}

    virtual void draw() override {}

private:
    SpriteAnimation* m_pAnimation;

    int m_nIdleAnimationIndex;
    int m_nWalkAnimationIndex;
};

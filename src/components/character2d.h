#include "sprite_animation.h"
#include "component.h"
#include "../input_handle.h"
#include "../debug_macro.h"

class Character2d : public Component
{
public:
    Character2d(SpriteAnimation* pAnimation) : m_pAnimation(pAnimation)
    {
        m_nIdleAnimationIndex = m_pAnimation->getAnimationIndexByName("idle");
        m_nWalkAnimationIndex = m_pAnimation->getAnimationIndexByName("walk");
        
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

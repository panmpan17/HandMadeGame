#include "sprite_animation.h"
#include "../component.h"
#include "../../core/debug_macro.h"

class Character2d : public Component
{
public:
    Character2d() {};
    Character2d(SpriteAnimation* pAnimation);


    ~Character2d()
    {
    }

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float fDeltaTime) override {}

    virtual void draw() override {}

private:
    SpriteAnimation* m_pAnimation = nullptr;

    int m_nIdleAnimationIndex = -1;
    int m_nWalkAnimationIndex = -1;

    COMPONENT_REGISTER_SERIALIZABLE(Character2d)
};

REGISTER_CLASS(Character2d)


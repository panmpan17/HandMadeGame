#include "../component.h"
#include "graphic_raycast_controller.h"

#include "../../core/math/vector.h"


class Sprite9Slice;


class UIButton : public NodeComponent, public IGraphicRaycastable
{
public:
    UIButton();
    ~UIButton();

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float fDeltaTime) override;

    virtual void getWorldBounds(Vector3& outTopLeft, Vector3& outBottomRight) const override;

    virtual void onMouseEnter() override;
    virtual void onMouseExit() override;
    virtual void onMouseClick() override;

    void setSize(const Vector2& size) { m_size = size; }
    const Vector2& getSize() const { return m_size; }

    void setSprite9Slice(Sprite9Slice* pSprite9Slice) { m_pSprite9Slice = pSprite9Slice; }
    Sprite9Slice* getSprite9Slice() const { return m_pSprite9Slice; }

private:
    // Vector2 m_centerOffset;
    Vector2 m_size;

    Sprite9Slice* m_pSprite9Slice = nullptr;

    
};

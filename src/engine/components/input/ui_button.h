#include "../component.h"
#include "graphic_raycast_controller.h"

#include "../../core/input/event.h"
#include "../../core/math/vector.h"
#include "../../core/math/color.h"


class IUIRaycastable;
class IColorable;


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
    virtual void onMouseDown() override;
    virtual void onMouseUp() override;
    virtual void onMouseClick() override;

    void setRaycastable(IUIRaycastable* pSprite9Slice) { m_pRaycastable = pSprite9Slice; }
    IUIRaycastable* getRaycastable() const { return m_pRaycastable; }

    void setColorable(IColorable* pColorable) { m_pColorable = pColorable; }
    IColorable* getColorable() const { return m_pColorable; }

    std::size_t registerOnClick(const std::function<void()>& listener) { return m_onClick.add(listener); }
    void unregisterOnClick(std::size_t nHandle) { m_onClick.remove(nHandle); }

private:
    Color m_normalColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
    Color m_hoverColor = Color(0.9f, 0.9f, 0.9f, 0.9f);
    Color m_clickColor = Color(0.8f, 0.8f, 0.8f, 0.8f);

    bool m_bMouseHover = false;
    bool m_bMouseDown = false;

    IUIRaycastable* m_pRaycastable = nullptr;
    IColorable* m_pColorable = nullptr;

    VoidEvent m_onClick;
};

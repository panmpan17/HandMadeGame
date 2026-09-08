#pragma once

#include "../component.h"

#include <vector>


struct Vector3;
struct Vector2;


class IGraphicRaycastable
{
public:
    bool isInteractable() const { return m_bInteractable; }

    // Get top left, bottom right position (in world)
    virtual void getWorldBounds(Vector3& outTopLeft, Vector3& outBottomRight) const = 0;

    virtual void onMouseEnter() {}
    virtual void onMouseExit() {}
    virtual void onMouseClick() {}
    
private:
    bool m_bInteractable = true;
};


class GraphicRaycastController : public NodeComponent
{
public:
    GraphicRaycastController();
    ~GraphicRaycastController();

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void onStart() override;
    virtual void update(float fDeltaTime) override;

    void registerRaycastable(IGraphicRaycastable* pRaycastable) { if (pRaycastable) m_vecRaycastables.push_back(pRaycastable); }
    void unregisterRaycastable(IGraphicRaycastable* pRaycastable) { if (pRaycastable) m_vecRaycastables.erase(std::remove(m_vecRaycastables.begin(), m_vecRaycastables.end(), pRaycastable), m_vecRaycastables.end()); }

private:
std::vector<IGraphicRaycastable*> m_vecRaycastables;
    IGraphicRaycastable* m_pCurrentHover = nullptr;
    std::size_t m_nMouseClickCallbackId = 0;


    void onMouseClickCheck(bool bPressed);

    IGraphicRaycastable* getCurrentMouseHoverObject() const;
};
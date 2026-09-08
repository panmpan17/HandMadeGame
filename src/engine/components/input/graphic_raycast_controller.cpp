#include "graphic_raycast_controller.h"

#include "../../core/debug_macro.h"
#include "../../core/input/input_manager.h"
#include "../../core/math/vector.h"
#include "../../core/camera.h"

#include <format>


GraphicRaycastController::GraphicRaycastController()
{}

GraphicRaycastController::~GraphicRaycastController()
{
    if (m_nMouseClickCallbackId != 0)
        InputManager::getInstance()->unregisterMouseButtonCallback(MouseButton::BUTTON_LEFT, m_nMouseClickCallbackId);
}

void GraphicRaycastController::onStart()
{
    LOGLN("GraphicRaycastController::onStart() called!");
    m_nMouseClickCallbackId = InputManager::getInstance()->registerMouseButtonCallback(
        MouseButton::BUTTON_LEFT, std::bind(&GraphicRaycastController::onMouseClickCheck, this, std::placeholders::_1));
}

void GraphicRaycastController::update(float fDeltaTime)
{
    IGraphicRaycastable* pCurrentHover = getCurrentMouseHoverObject();
    if (pCurrentHover != m_pCurrentHover)
    {
        if (m_pCurrentHover)
            m_pCurrentHover->onMouseExit();
        if (pCurrentHover)
            pCurrentHover->onMouseEnter();
        m_pCurrentHover = pCurrentHover;
    }
}



void GraphicRaycastController::onMouseClickCheck(bool bPressed)
{
    if (!bPressed) return;

    IGraphicRaycastable* pCurrentHover = getCurrentMouseHoverObject();
    if (pCurrentHover)
    {
        pCurrentHover->onMouseClick();
    }
}

IGraphicRaycastable* GraphicRaycastController::getCurrentMouseHoverObject() const
{
    Vector2 oScreenPos;
    InputManager::getInstance()->getMousePosition(oScreenPos.x, oScreenPos.y);

    Camera* pMainCamera = Camera::main;

    Vector3 oTopLeft, oBottomRight;
    for (int i = 0; i < m_vecRaycastables.size(); ++i)
    {
        IGraphicRaycastable* pRaycastable = m_vecRaycastables[i];
        pRaycastable->getWorldBounds(oTopLeft, oBottomRight);

        oTopLeft = pMainCamera->worldPositionToScreenPosition(oTopLeft);
        oBottomRight = pMainCamera->worldPositionToScreenPosition(oBottomRight);

        if (oScreenPos.x >= oTopLeft.x && oScreenPos.x <= oBottomRight.x &&
            oScreenPos.y >= oTopLeft.y && oScreenPos.y <= oBottomRight.y)
        {
            return pRaycastable;
        }
    }
    return nullptr;
}

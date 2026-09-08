#include "ui_button.h"

#include "../../core/scene/node.h"
#include "../../core/debug_macro.h"
#include "../render/sprite_9slice.h"


UIButton::UIButton()
{}

UIButton::~UIButton()
{
    // GraphicRaycastController* pRaycastController = getNode()->getComponent<GraphicRaycastController>();
    // if (pRaycastController)
    //     pRaycastController->unregisterRaycastable(this);
}

void UIButton::update(float fDeltaTime)
{
    // Implement any update logic for the UIButton here
}

void UIButton::getWorldBounds(Vector3& outTopLeft, Vector3& outBottomRight) const
{
    Node* pNode = getNode();
    const Vector3& center = pNode->getPosition();

    outTopLeft = pNode->transformPoint(Vector3(center.x - m_size.x / 2.0f, center.y + m_size.y / 2.0f, center.z));
    outBottomRight = pNode->transformPoint(Vector3(center.x + m_size.x / 2.0f, center.y - m_size.y / 2.0f, center.z));
}

void UIButton::onMouseEnter()
{
    // Implement mouse enter behavior here
    if (m_pSprite9Slice)
    {
        // TODO: Set hover color
        LOGLN("Mouse entered UIButton");
    }
}

void UIButton::onMouseExit()
{
    // Implement mouse exit behavior here
    if (m_pSprite9Slice)
    {
        // TODO: Reset to normal color
        LOGLN("Mouse exited UIButton");
    }
}

void UIButton::onMouseClick()
{
    // Implement mouse click behavior here
    LOGLN("Mouse clicked UIButton");
}
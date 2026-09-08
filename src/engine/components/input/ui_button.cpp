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

    // TODO: I tested button moving when clicked in minimal_metal_test.cpp, but the bounding box is weird, check it out
    outTopLeft = pNode->transformPoint(Vector3(center.x - m_size.x / 2.0f, center.y + m_size.y / 2.0f, center.z));
    outBottomRight = pNode->transformPoint(Vector3(center.x + m_size.x / 2.0f, center.y - m_size.y / 2.0f, center.z));
}

void UIButton::onMouseEnter()
{
    m_bMouseHover = true;

    if (m_pSprite9Slice)
    {
        m_pSprite9Slice->setColor(m_bMouseDown ? m_clickColor : m_hoverColor);
    }
}

void UIButton::onMouseExit()
{
    m_bMouseHover = false;

    if (m_pSprite9Slice)
    {
        m_pSprite9Slice->setColor(m_normalColor);
    }
}

void UIButton::onMouseDown()
{
    if (m_pSprite9Slice)
    {
        m_pSprite9Slice->setColor(m_clickColor);
    }
    m_bMouseDown = true;
}

void UIButton::onMouseUp()
{
    if (m_pSprite9Slice)
    {
        m_pSprite9Slice->setColor(m_bMouseHover ? m_hoverColor : m_normalColor);
    }
    m_bMouseDown = false;
}

void UIButton::onMouseClick()
{
    m_onClick.invoke();
}
#include "ui_button.h"

#include "../../core/scene/node.h"
#include "../../core/debug_macro.h"
#include "../render/ui_raycastable.h"
#include "../drawable_interface.h"


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
    if (m_pRaycastable) {
        float fWidth = m_pRaycastable->getWidth();
        float fHeight = m_pRaycastable->getHeight();
        outTopLeft = pNode->transformPoint(Vector3(-fWidth / 2.0f, fHeight / 2.0f, 0));
        outBottomRight = pNode->transformPoint(Vector3(fWidth / 2.0f, -fHeight / 2.0f, 0));
    }
    else
    {
        outTopLeft = pNode->getPositionInWorld();
        outBottomRight = pNode->getPositionInWorld();
    }
}

void UIButton::onMouseEnter()
{
    m_bMouseHover = true;

    if (m_pColorable)
    {
        m_pColorable->setColor(m_bMouseDown ? m_clickColor : m_hoverColor);
    }
}

void UIButton::onMouseExit()
{
    m_bMouseHover = false;

    if (m_pColorable)
    {
        m_pColorable->setColor(m_normalColor);
    }
}

void UIButton::onMouseDown()
{
    if (m_pColorable)
    {
        m_pColorable->setColor(m_clickColor);
    }
    m_bMouseDown = true;
}

void UIButton::onMouseUp()
{
    if (m_pColorable)
    {
        m_pColorable->setColor(m_bMouseHover ? m_hoverColor : m_normalColor);
    }
    m_bMouseDown = false;
}

void UIButton::onMouseClick()
{
    m_onClick.invoke();
}
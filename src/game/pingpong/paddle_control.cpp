#include "paddle_control.h"
#include "../../input_handle.h"
#include "../../node.h"
#include "../../components/quad.h"
#include "../../draw/shader_loader.h"
#include "../../debug_macro.h"

#define BIND_CALLBACK_1(func) std::bind(&PaddleControl::func, this, std::placeholders::_1)


PaddleControl::PaddleControl(const Box& oBox, PaddleControlType eControlType, float fMaxSpeed) : m_oBox(oBox), m_eControlType(eControlType), m_fMaxSpeed(fMaxSpeed)
{
    switch (m_eControlType)
    {
    case PaddleControlType::PLAYER1:
        InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_W, BIND_CALLBACK_1(onUpPressCallback));
        InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_S, BIND_CALLBACK_1(onDownPressCallback));
        break;
    case PaddleControlType::PLAYER2:
        InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_O, BIND_CALLBACK_1(onUpPressCallback));
        InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_L, BIND_CALLBACK_1(onDownPressCallback));
        break;

    default:
        break;
    }
}

PaddleControl::~PaddleControl()
{
    // InputManager::getInstance()->unregisterKeyPressCallback(KeyCode::KEY_W);
    // InputManager::getInstance()->unregisterKeyPressCallback(KeyCode::KEY_S);
}

Box PaddleControl::getBox() const 
{
    return m_oBox + m_pNode->getPosition();
}

void PaddleControl::start()
{
    Shader* pImageShader = ShaderLoader::getInstance()->getShader("image");

    auto pQuad = new Quad(m_oBox.getSizeX(), m_oBox.getSizeY(), vec4{ 1.f, 1.f, 1.f, 1.f });
    pQuad->setShader(pImageShader);
    pQuad->registerBuffer();
    m_pNode->addComponent(pQuad);
}

void PaddleControl::onUpPressCallback(bool bPressed)
{
    m_bUpPressed = bPressed;
}

void PaddleControl::onDownPressCallback(bool bPressed)
{
    m_bDownPressed = bPressed;
}

void PaddleControl::update(float fDeltaTime)
{
    if (m_eControlType == PaddleControlType::AI)
    {
        updateAI(fDeltaTime);
    }

    float fMoveDistance = m_fMaxSpeed * fDeltaTime;
    float posY = m_pNode->getPositionY();
    if (m_bUpPressed && !m_bDownPressed)
    {
        posY += fMoveDistance;

        if (posY > m_fPositionMaxY) posY = m_fPositionMaxY;
        m_pNode->setPosition(m_pNode->getPositionX(), posY);
    }
    else if (m_bDownPressed && !m_bUpPressed)
    {
        posY -= fMoveDistance;

        if (posY < m_fPositionMinY) posY = m_fPositionMinY;
        m_pNode->setPosition(m_pNode->getPositionX(), posY);
    }
}

void PaddleControl::updateAI(float fDeltaTime)
{
    if (!m_pPong) return;

    float fBallY = m_pPong->getPositionY();

    if (fBallY > m_pNode->getPositionY() + 0.5)
    {
        m_bUpPressed = true;
        m_bDownPressed = false;
    }
    else if (fBallY < m_pNode->getPositionY() - 0.5)
    {
        m_bUpPressed = false;
        m_bDownPressed = true;
    }
    else if (fabs(fBallY - m_pNode->getPositionY()) <= 0.03f)
    {
        m_bUpPressed = false;
        m_bDownPressed = false;
    }
}

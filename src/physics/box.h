#pragma once

#include "../vector.h"


struct Box
{
    float m_fStartX;
    float m_fStartY;
    float m_fEndX;
    float m_fEndY;

    Box() = default;
    Box(float fStartX, float fStartY, float fEndX, float fEndY)
        : m_fStartX(fStartX), m_fStartY(fStartY), m_fEndX(fEndX), m_fEndY(fEndY) {}
    Box(const Vector2& vecPosition, const Vector2& vecSize)
        : m_fStartX(vecPosition.x), m_fStartY(vecPosition.y),
          m_fEndX(vecPosition.x + vecSize.x), m_fEndY(vecPosition.y + vecSize.y) {}

    static Box fromCenter(float fCenterX, float fCenterY, float fWidth, float fHeight)
    {
        return Box(fCenterX - (fWidth / 2.0f), fCenterY - (fHeight / 2.0f), fCenterX + (fWidth / 2.0f), fCenterY + (fHeight / 2.0f));
    }
    static Box fromCenter(const Vector2& vecCenter, const Vector2& vecSize)
    {
        return Box(vecCenter.x - (vecSize.x / 2.0f), vecCenter.y - (vecSize.y / 2.0f),
                   vecCenter.x + (vecSize.x / 2.0f), vecCenter.y + (vecSize.y / 2.0f));
    }
    static Box fromCenter(const Vector2& vecCenter, float fWidth, float fHeight)
    {
        return Box(vecCenter.x - (fWidth / 2.0f), vecCenter.y - (fHeight / 2.0f),
                   vecCenter.x + (fWidth / 2.0f), vecCenter.y + (fHeight / 2.0f));
    }

    Vector2 getPosition() const { return Vector2{m_fStartX, m_fStartY}; }
    Vector2 getSize() const { return Vector2{m_fEndX - m_fStartX, m_fEndY - m_fStartY}; }
    float getSizeX() const { return m_fEndX - m_fStartX; }
    float getSizeY() const { return m_fEndY - m_fStartY; }
};

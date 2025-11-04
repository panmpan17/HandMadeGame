#pragma once

#include <format>
#include "../core/math/vector.h"


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

    Box operator+(const Vector2& vecOther) const { return Box(m_fStartX + vecOther.x, m_fStartY + vecOther.y, m_fEndX + vecOther.x, m_fEndY + vecOther.y); }
    Box operator+(const Vector3& vecOther) const { return Box(m_fStartX + vecOther.x, m_fStartY + vecOther.y, m_fEndX + vecOther.x, m_fEndY + vecOther.y); }

    std::string getString() const
    {
         return std::format("Box(start=({}, {}), end=({}, {}))",
            m_fStartX, m_fStartY, m_fEndX, m_fEndY );
    }
};


// template<>
// struct std::formatter<Box> : std::formatter<std::string> {
//     // Parse the format specifier (e.g., "{:N}" for name only, "{:V}" for value only)
//     // This example supports simple specifiers for demonstration.
//     char presentation_format = 'F'; // Default to full format

//     auto parse(format_parse_context& ctx) {
//         auto it = ctx.begin();
//         if (it != ctx.end() && *it != '}') {
//             presentation_format = *it;
//             ++it;
//         }
//         return it;
//     }

//     // Format the Box object based on the parsed specifier
//     auto format(const Box& obj, format_context& ctx) const {
//         return std::format_to(ctx.out(), "Box(start=({}, {}), end=({}, {}))",
//             obj.m_fStartX, obj.m_fStartY, obj.m_fEndX, obj.m_fEndY );
//     }
// };
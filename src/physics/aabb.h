#pragma once

#include "box.h"
#include "../debug_macro.h"


class AABB
{
public:
    static bool isColliding(const Box& boxA, const Box& boxB)
    {
        return (boxA.m_fStartX < boxB.m_fEndX && boxA.m_fEndX > boxB.m_fStartX &&
                boxA.m_fStartY < boxB.m_fEndY && boxA.m_fEndY > boxB.m_fStartY);
    }
};


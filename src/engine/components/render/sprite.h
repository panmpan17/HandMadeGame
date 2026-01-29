#pragma once

#include "quad.h"

class Sprite : public Quad
{
public:
    Sprite() {}
    Sprite(Image* pImage, int nPixelPerUnit = 100);
    Sprite(Image* pImage, int nSpriteSheetXCount, int nSpriteSheetYCount, int nSpriteIndex = 0, int nPixelPerUnit = 100);

protected:
    COMPONENT_REGISTER_SERIALIZABLE(Sprite)
};

REGISTER_CLASS(Sprite)

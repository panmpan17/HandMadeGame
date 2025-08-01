#pragma once

#include "../components/component.h"


class Shader;

class IDrawable : public Component
{
public:
    virtual ~IDrawable() = default;

    virtual bool isIDrawable() override { return true; }
    virtual bool isIUpdatable() override { return false; }

    virtual void registerBuffer() = 0;
    virtual void draw() = 0;
    virtual void setShader(Shader* pShader) = 0;
};


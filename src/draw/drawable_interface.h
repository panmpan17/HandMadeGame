#pragma once

#include "../components/component.h"


class Shader;

class IDrawable : public Component
{
public:
    virtual ~IDrawable() = default;

    virtual bool isIDrawable() const override { return true; }
    virtual bool isUpdatable() const override { return false; }

    virtual void registerBuffer() = 0;
    virtual void setShader(Shader* pShader) = 0;

    virtual void update(float deltaTime) override {}
};


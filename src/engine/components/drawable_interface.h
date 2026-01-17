#pragma once

#include <memory>
#include "../components/component.h"
#include "../render/material.h"


class Shader;
class Material;

class IDrawable : public NodeComponent
{
public:
    virtual ~IDrawable() = default;

    virtual bool isIDrawable() const override { return true; }
    virtual bool isUpdatable() const override { return false; }

    virtual void setShader(Shader* pShader) = 0;

    virtual void update(float _) override {}

    virtual void draw() = 0;
    virtual void drawDepth() {}

    bool getIsTransparent() const
    {
        if (m_pMaterial)
        {
            return m_pMaterial->getIsTransparent();
        }
        return false;
    }

protected:
    std::shared_ptr<Material> m_pMaterial = nullptr;
};


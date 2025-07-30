#pragma once

#include "vertex.h"
#include "shader.h"
#include "drawable_interface.h"


class Triangle : public IDrawable
{
public:
    Triangle();
    ~Triangle();

    void registerBuffer() override;
    void draw() override;

    inline void setShader(Shader* pShader) override { m_pShader = static_cast<TestShader*>(pShader); }

private:
    TestShader* m_pShader = nullptr;
    VertexWColor vertices[3];
};
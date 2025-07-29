#pragma once

#include "shader.h"


class IDrawable
{
public:
    virtual void draw() = 0;
};


class Triangle : public IDrawable
{
public:
    Triangle();
    ~Triangle();

    void registerBuffer();
    void draw() override;

    void setShader(TestShader* shader) { m_pShader = shader; }

private:
    TestShader* m_pShader = nullptr;
    VertexWColor vertices[3];
};
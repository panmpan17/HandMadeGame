#pragma once

#include "vertex.h"
#include "shader.h"
#include "drawable_interface.h"

typedef unsigned int GLuint;

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

    GLuint m_nVertexBuffer, m_nVertexArray;
};
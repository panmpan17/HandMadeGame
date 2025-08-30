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

    inline void setShader(Shader* pShader) override { m_pShader = pShader; m_nMVPUniform = m_pShader->getUniformLocation("u_MVP"); }

private:
    Shader* m_pShader = nullptr;

    GLuint m_nVertexBuffer, m_nVertexArray, m_nMVPUniform;
};
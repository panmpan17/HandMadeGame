#pragma once

#include "vertex.h"
#include "shader.h"
#include "drawable_interface.h"

typedef unsigned int GLuint;

class Quad : public IDrawable
{
public:
    Quad(float fX, float fY, float fWidth, float fHeight, vec3 color);
    ~Quad();

    void registerBuffer() override;
    void draw() override;

    inline void setShader(Shader* pShader) override { m_pShader = static_cast<TestShader*>(pShader); }

private:
    GLuint m_vertexBuffer;
    GLuint m_vertexArray;

    VertexWColor m_arrVertices[4];

    TestShader* m_pShader = nullptr;
};

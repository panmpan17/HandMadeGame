#pragma once

#include "shader.h"

typedef unsigned int GLuint;

class Quad
{
public:
    Quad(float fX, float fY, float fWidth, float fHeight, vec3 color);
    ~Quad();

    void draw();

    inline void setShaderProgram(TestShader* shader) { m_pShader = shader; }

private:
    GLuint m_vertexBuffer;
    GLuint m_vertexArray;

    VertexWColor m_arrVertices[4];

    TestShader* m_pShader = nullptr;

    void setupVertexBuffer();
};

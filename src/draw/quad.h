#pragma once

#include <linmath.h>

#include "vertex.h"
#include "shader.h"
#include "drawable_interface.h"

typedef unsigned int GLuint;

class Quad : public IDrawable
{
public:
    Quad(float fX, float fY, float fWidth, float fHeight, vec4 color);
    ~Quad();

    void registerBuffer() override;
    void draw() override;

    inline void setPosition(float fX, float fY) {
        m_position[0] = fX;
        m_position[1] = fY;
    }

    inline void setShader(Shader* pShader) override { m_pShader = static_cast<ImageShader*>(pShader); }

private:
    vec3 m_position = {0.f, 0.f, 0.f};

    GLuint m_nVertexBuffer, m_nVertexArray;

    VertexWUV m_arrVertices[4];
    vec4 m_color = {1.f, 1.f, 1.f, 1.f};

    ImageShader* m_pShader = nullptr;
};

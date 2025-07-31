#pragma once

#include <linmath.h>

#include "vertex.h"
#include "shader.h"
#include "drawable_interface.h"

typedef unsigned int GLuint;

class Image;

class Quad : public IDrawable
{
public:
    Quad(float fX, float fY, float fWidth, float fHeight, vec4 color);
    ~Quad();

    void registerBuffer() override;
    void draw() override;

    inline void setShader(Shader* pShader) override { m_pShader = static_cast<ImageShader*>(pShader); }
    inline void setImage(Image* pImage) { m_pImage = pImage; }

private:
    GLuint m_nVertexBuffer, m_nVertexArray;

    VertexWUV m_arrVertices[4];
    vec4 m_color = {1.f, 1.f, 1.f, 1.f};

    ImageShader* m_pShader = nullptr;
    Image* m_pImage = nullptr; // Optional, if the quad uses an image texture
};

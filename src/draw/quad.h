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
    Quad(float fWidth, float fHeight, vec4 color);
    ~Quad();

    void registerBuffer() override;
    void draw() override;

    inline void setShader(Shader* pShader) override { m_pShader = static_cast<ImageShader*>(pShader); }
    inline void setImage(Image* pImage) { m_pImage = pImage; }

protected:
    Quad() {};

    virtual void predrawSetShaderUniforms();

    GLuint m_nVertexBuffer, m_nVertexArray;

    float m_fWidth, m_fHeight;
    vec4 m_color = {1.f, 1.f, 1.f, 1.f};

    ImageShader* m_pShader = nullptr;
    Image* m_pImage = nullptr; // Optional, if the quad uses an image texture

    virtual void serializeToWrapper(DataSerializer& serializer) const override;
};

class Sprite : public Quad
{
public:
    Sprite(Image* pImage, int nPixelPerUnit = 100);
    Sprite(Image* pImage, int nSpriteSheetXCount, int nSpriteSheetYCount, int nSpriteIndex = 0, int nPixelPerUnit = 100);
    ~Sprite();

    void draw() override;

    void setSpriteIndex(int nIndex);
    int getSpriteIndex() const { return m_nSpriteIndex; }

protected:
    void predrawSetShaderUniforms() override;
    int m_nSpriteSheetXCount = 1;
    int m_nSpriteSheetYCount = 1;
    int m_nSpriteIndex = 0;
    vec2 m_vecUVOOffset;
};

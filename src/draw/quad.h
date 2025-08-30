#pragma once

#include <linmath.h>

#include "vertex.h"
#include "drawable_interface.h"

typedef unsigned int GLuint;

class Image;
class Shader;

class Quad : public IDrawable
{
public:
    Quad() {}
    Quad(float fWidth, float fHeight, vec4 color);
    ~Quad();

    void registerBuffer() override;
    void draw() override;

    virtual void setShader(Shader* pShader) override;
    inline void setImage(Image* pImage) { m_pImage = pImage; }

    virtual void deserializeField(const std::string_view& strFieldName, const std::string_view& strFieldValue) override;
    virtual void onNodeFinishedDeserialization() override;

protected:
    virtual void predrawSetShaderUniforms();

    GLuint m_nVertexBuffer, m_nVertexArray;
    GLuint m_nSpriteXCountUniform, m_nSpriteYCountUniform, m_nUVOffsetUniform;
    GLuint m_nMVPUniform, m_nColorUniform, m_nTextureUniform;

    float m_fWidth, m_fHeight;
    vec4 m_color = {1.f, 1.f, 1.f, 1.f};

    Shader* m_pShader = nullptr;
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

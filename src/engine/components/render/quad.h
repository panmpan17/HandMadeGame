#pragma once

#include <linmath.h>

#include "../drawable_interface.h"
#include "../../render/shader.h"
#include "../../render/vertex.h"
#include "../../core/math/vector.h"

#include "../../core/debug_macro.h"

#if __APPLE__
#include <Metal/Metal.hpp>
#endif // __APPLE__

typedef unsigned int GLuint;

class Image;
class Shader;

class Quad : public IDrawable
{
public:
    Quad() {}
    Quad(float fWidth, float fHeight, vec4 color);
    ~Quad();

    void registerBuffer();
    void draw() override;

    virtual void setShader(Shader* pShader) override;
    inline void setImage(Image* pImage) { m_pImage = pImage; }

    inline void getColor(vec4& outColor) const { vec4_dup(outColor, m_color); }
    inline void setColor(const vec4 color) { vec4_dup(m_color, color); }
    inline void setColor(float r, float g, float b, float a) { m_color[0] = r; m_color[1] = g; m_color[2] = b; m_color[3] = a; }

    bool getIsTransparent() const override;

    inline void setSpriteIndex(int nIndex)
    {
        m_nSpriteIndex = nIndex % (m_vecSpriteSheetCount.x * m_vecSpriteSheetCount.y);
        int xIndex = m_nSpriteIndex % m_vecSpriteSheetCount.x;
        int yIndex = m_nSpriteIndex / m_vecSpriteSheetCount.x;
        m_vecUVOOffset[0] = static_cast<float>(xIndex) / static_cast<float>(m_vecSpriteSheetCount.x);
        m_vecUVOOffset[1] = static_cast<float>(yIndex) / static_cast<float>(m_vecSpriteSheetCount.y);
    }
    inline int getSpriteIndex() const { return m_nSpriteIndex; }

protected:
    GLuint m_nVertexBuffer, m_nVertexArray;

#if __APPLE__
    MTL::Buffer* m_pVertexBuffer = nullptr;
#endif // __APPLE__

    const ShaderUniformHandle* m_pMVPHandle = nullptr;
    const ShaderUniformHandle* m_pColorHandle = nullptr;
    const ShaderUniformHandle* m_pTextureHandle = nullptr;
    const ShaderUniformHandle* m_pUseTextureHandle = nullptr;
    const ShaderUniformHandle* m_pSpriteSheetXCountHandle = nullptr;
    const ShaderUniformHandle* m_pSpriteSheetYCountHandle = nullptr;
    const ShaderUniformHandle* m_pUVOffsetHandle = nullptr;

    float m_fWidth, m_fHeight;
    vec4 m_color = {1.f, 1.f, 1.f, 1.f};

    Shader* m_pShader = nullptr;
    Image* m_pImage = nullptr; // Optional, if the quad uses an image texture

    int m_nSpriteIndex = 0;
    vec2 m_vecUVOOffset = {0.f, 0.f};
    Vector2i m_vecSpriteSheetCount = Vector2i(1.f, 1.f);

    COMPONENT_REGISTER_SERIALIZABLE(Quad)
public:
    virtual void onNodeFinishedDeserialization() override;
};

REGISTER_CLASS(Quad)

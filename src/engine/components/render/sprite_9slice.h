#pragma once

#include <linmath.h>

#include "../drawable_interface.h"
// #include "../../render/shader.h"
// #include "../../render/vertex.h"
#include "../../core/math/color.h"
#include "../../core/math/vector.h"

// #include "../../core/debug_macro.h"

#if __APPLE__
#include <Metal/Metal.hpp>
#endif // __APPLE__

class Shader;
class Image;

typedef unsigned int GLuint;

struct Slice9
{
    float fPixelOnLeftEdge = 0;
    float fPixelOnRightEdge = 1;
    float fPixelOnTopEdge = 0;
    float fPixelOnBottomEdge = 1;
};

class Sprite9Slice : public IDrawable
{
public:
    Sprite9Slice() {}
    Sprite9Slice(Image* pImage, float fWidth, float fHeight, float fPixelPerUnit, const Slice9& slice, int nPixelPerUnit = 100);
    ~Sprite9Slice();

    void registerBuffer();
    void draw() override;
    
    // Not allow setting shader for 9-slice sprite, as it uses a specific shader for 9-slice rendering
    virtual void setShader(Shader* pShader) override {}
    inline void setImage(Image* pImage) { m_pImage = pImage; }

    bool getIsTransparent() const override { return true; }

private:
    // Color m_color;
    Image* m_pImage = nullptr;
    Shader* m_pShader = nullptr;
    Slice9 m_slice;

    const ShaderUniformHandle* m_pMVPHandle = nullptr;
    const ShaderUniformHandle* m_pTextureHandle = nullptr;
    const ShaderUniformHandle* m_pSliceDataHandle = nullptr;

    GLuint m_nVertexBuffer;
    GLuint m_nVertexArray;

    float m_fWidth, m_fHeight, m_fPixelPerUnit;
};

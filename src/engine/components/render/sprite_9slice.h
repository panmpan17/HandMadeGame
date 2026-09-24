#pragma once

#include <linmath.h>

#include "../drawable_interface.h"
#include "../../render/gl_macro.h"
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

struct Slice9
{
    float fPixelOnLeftEdge = 0;
    float fPixelOnRightEdge = 1;
    float fPixelOnTopEdge = 0;
    float fPixelOnBottomEdge = 1;
};

enum Sprite9Slice_SizeCompensateMode
{
    NONE = 0,
    MIDDLE = 1,
    EDGE1 = 2,
    EDGE2 = 3,
};

class Sprite9Slice : public IDrawable
{
public:
    Sprite9Slice() {}
    Sprite9Slice(Image* pImage, float fWidth, float fHeight, float fPixelPerUnit, const Slice9& slice, int nPixelPerUnit = 100);
    ~Sprite9Slice();

    void draw() override;
    
    // Not allow setting shader for 9-slice sprite, as it uses a specific shader for 9-slice rendering
    virtual void setShader(Shader* pShader) override {}
    inline void setImage(Image* pImage) { m_pImage = pImage; }

    bool getIsTransparent() const override { return true; }

    float getWidth() const { return m_fWidth; }
    float getHeight() const { return m_fHeight; }
    void setSize(float fWidth, float fHeight)
    {
        m_fWidth = fWidth;
        m_fHeight = fHeight;
        m_bBufferDirty = true;
    }

    void setColor(const Color& color) { m_color = color; }
    const Color& getColor() const { return m_color; }

    void setWidthCompensateMode(Sprite9Slice_SizeCompensateMode eMode) { m_eWidthCompensateMode = eMode; m_bBufferDirty = true; }
    void setHeightCompensateMode(Sprite9Slice_SizeCompensateMode eMode) { m_eHeightCompensateMode = eMode; m_bBufferDirty = true; }
    Sprite9Slice_SizeCompensateMode getWidthCompensateMode() const { return m_eWidthCompensateMode; }
    Sprite9Slice_SizeCompensateMode getHeightCompensateMode() const { return m_eHeightCompensateMode; }

private:
    Color m_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    Image* m_pImage = nullptr;
    Shader* m_pShader = nullptr;
    Slice9 m_slice;

    const ShaderUniformHandle* m_pMVPHandle = nullptr;
    const ShaderUniformHandle* m_pTextureHandle = nullptr;
    const ShaderUniformHandle* m_pSliceDataHandle = nullptr;
    const ShaderUniformHandle* m_pColorHandle = nullptr;

    GLuint m_nVertexBuffer = GL_INVALID_INDEX;
    GLuint m_nVertexArray = GL_INVALID_INDEX;
    bool m_bBufferDirty = true;

    float m_fWidth, m_fHeight, m_fPixelPerUnit;

    Sprite9Slice_SizeCompensateMode m_eWidthCompensateMode = Sprite9Slice_SizeCompensateMode::MIDDLE;
    Sprite9Slice_SizeCompensateMode m_eHeightCompensateMode = Sprite9Slice_SizeCompensateMode::MIDDLE;

    void registerBuffer();
};

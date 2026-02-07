// #include <memory>
#include "../component.h"
#include "../drawable_interface.h"

class Shader;
class Mesh;
class Material;
class Font;


typedef unsigned int GLuint;


class TextRenderer : public IDrawable
{
public:
    TextRenderer(Font* const pFont);
    ~TextRenderer();

    virtual void update(float fDeltaTime) override {}

    virtual void draw() override;

    virtual void setShader(Shader* pShader) override;

    virtual NodeComponent* clone() const override
    {
        TextRenderer* pNewTextRenderer = new TextRenderer(m_pFont);
        if (m_pMaterial) pNewTextRenderer->setMaterial(m_pMaterial);
        return pNewTextRenderer;
    }

    void setMaterial(const std::shared_ptr<Material>& pMaterial);

    inline const std::u16string& getText() const { return m_strText; }
    inline void setText(const std::u16string& strText) { m_strText = strText; }

    inline const float getFontSize() const { return m_fFontSize; }
    inline void setFontSize(float fFontSize) { m_fFontSize = fFontSize; }

    virtual bool getIsTransparent() const override { return true; }

private:
    GLuint m_nVertexArray = 0;
    GLuint m_nVertexBuffer = 0;

    Shader* m_pShader = nullptr;
    Font* m_pFont = nullptr;

    const ShaderUniformHandle* m_pMVPHandle = nullptr;
    const ShaderUniformHandle* m_pTextureHandle = nullptr;
    const ShaderUniformHandle* m_pSizeHandle = nullptr;

    std::u16string m_strText;
    float m_fFontSize = 1.f;

    void bindVertexArray(Shader* const pShader);

    // COMPONENT_REGISTER_SERIALIZABLE(TextRenderer)
};

// REGISTER_CLASS(TextRenderer)


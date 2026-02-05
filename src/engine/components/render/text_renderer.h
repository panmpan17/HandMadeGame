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

private:
    GLuint m_nVertexArray = 0;
    GLuint m_nVertexBuffer = 0;

    Shader* m_pShader = nullptr;

    Font* m_pFont = nullptr;

    const ShaderUniformHandle* m_pMVPHandle = nullptr;
    const ShaderUniformHandle* m_pTextureHandle = nullptr;


    int m_nIndiceCount = 0;

    void bindVertexArray(Shader* const pShader);

    // int m_nIdleAnimationIndex = -1;
    // int m_nWalkAnimationIndex = -1;

    // COMPONENT_REGISTER_SERIALIZABLE(TextRenderer)
};

// REGISTER_CLASS(TextRenderer)


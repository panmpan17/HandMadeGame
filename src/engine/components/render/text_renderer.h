// #include <memory>
#include "../component.h"
#include "../drawable_interface.h"

class Shader;
class Mesh;
class Material;


typedef unsigned int GLuint;


class TextRenderer : public IDrawable
{
public:
    TextRenderer();
    ~TextRenderer();

    virtual void update(float fDeltaTime) override {}

    virtual void draw() override;

    virtual void setShader(Shader* pShader) override {}

    virtual NodeComponent* clone() const override
    {
        TextRenderer* pNewTextRenderer = new TextRenderer();
        if (m_pMaterial) pNewTextRenderer->setMaterial(m_pMaterial);
        return pNewTextRenderer;
    }

    void setMaterial(const std::shared_ptr<Material>& pMaterial);

private:
    GLuint m_nVertexArray = 0;
    GLuint m_nVertexBuffer = 0;

    const ShaderUniformHandle* m_pModelUniform = nullptr;
    // const ShaderUniformHandle* m_pSpecularParamUniform = nullptr;
    // const ShaderUniformHandle* m_pTextureEnabledUniform = nullptr;

    // const ShaderUniformHandle* m_pDepthTextureUniform = nullptr;
    // const ShaderUniformHandle* m_pLightMatrixUniform1 = nullptr;
    // const ShaderUniformHandle* m_pShadowColorUniform = nullptr;

    // Shader* m_pDepthShader = nullptr;
    // const ShaderUniformHandle* m_pDepthModelUniform = nullptr;
    // const ShaderUniformHandle* m_pLightMatrixUniform2 = nullptr;


    int m_nIndiceCount = 0;

    // void bindVertexArray(Shader* const pShader);

    // int m_nIdleAnimationIndex = -1;
    // int m_nWalkAnimationIndex = -1;

    // COMPONENT_REGISTER_SERIALIZABLE(TextRenderer)
};

// REGISTER_CLASS(TextRenderer)


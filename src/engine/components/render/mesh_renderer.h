#include <memory>
#include "../component.h"
#include "../drawable_interface.h"
#include "../../render/shader.h"

class SimpleObjReader;
class Shader;
class Image;
class Mesh;
class Material;


typedef unsigned int GLuint;


class MeshRenderer : public IDrawable
{
public:
    MeshRenderer();
    ~MeshRenderer();
    virtual void update(float fDeltaTime) override {}

    virtual void draw() override;
    virtual void drawDepth() override;

    virtual Component* clone() const override
    {
        MeshRenderer* pNewMeshRenderer = new MeshRenderer();
        if (m_pMesh) pNewMeshRenderer->setMesh(m_pMesh);
        if (m_pMaterial) pNewMeshRenderer->setMaterial(m_pMaterial);
        return pNewMeshRenderer;
    }

    void setShader(Shader* pShader) override {}

    void setMaterial(const std::shared_ptr<Material>& pMaterial);
    inline void setMesh(const std::shared_ptr<Mesh>& pMesh) { m_pMesh = pMesh; }

    void initShader(Shader* const pShader);

private:
    std::shared_ptr<Mesh> m_pMesh = nullptr;

    GLuint m_nVertexArray = 0;
    GLuint m_nVertexBuffer = 0;
    GLuint m_nIndexBuffer = 0;

    const ShaderUniformHandle* m_pModelUniform = nullptr;
    const ShaderUniformHandle* m_pSpecularParamUniform = nullptr;
    const ShaderUniformHandle* m_pTextureEnabledUniform = nullptr;

    const ShaderUniformHandle* m_pDepthTextureUniform = nullptr;
    const ShaderUniformHandle* m_pLightMatrixUniform1 = nullptr;
    const ShaderUniformHandle* m_pShadowColorUniform = nullptr;

    Shader* m_pDepthShader = nullptr;
    const ShaderUniformHandle* m_pDepthModelUniform = nullptr;
    const ShaderUniformHandle* m_pLightMatrixUniform2 = nullptr;


    int m_nIndiceCount = 0;

    void bindVertexArray(Shader* const pShader);
    void bindDepthVertexArray();

    // int m_nIdleAnimationIndex = -1;
    // int m_nWalkAnimationIndex = -1;

    COMPONENT_REGISTER_SERIALIZABLE(MeshRenderer)
};

REGISTER_CLASS(MeshRenderer)


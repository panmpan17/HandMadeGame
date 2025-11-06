#include <memory>
#include "../component.h"
#include "../../render/shader.h"

class SimpleObjReader;
class Shader;
class Image;
class Mesh;
class Material;


typedef unsigned int GLuint;


class MeshRenderer : public Component
{
public:
    MeshRenderer();
    ~MeshRenderer();

    virtual bool isIDrawable() const override { return true; }
    virtual bool isUpdatable() const override { return false; }

    virtual void update(float fDeltaTime) override {}

    virtual void draw() override;
    virtual void drawDepth() override;

    void setShader(Shader* pShader);

    void setMaterial(std::shared_ptr<Material>& pMaterial);
    inline void setMesh(const std::shared_ptr<Mesh>& pMesh) { m_pMesh = pMesh; }
    inline void setMainTexture(Image* pImage) { m_pMainTexture = pImage; }
    inline void setSpecularMap(Image* pImage) { m_pSpecularMap = pImage; }

    void initShader(Shader* const pShader);

private:
    std::shared_ptr<Mesh> m_pMesh = nullptr;

    std::shared_ptr<Material> m_pMaterial = nullptr;

    Image* m_pMainTexture = nullptr;
    Image* m_pSpecularMap = nullptr;

    Shader* m_pShader = nullptr;

    GLuint m_nVertexArray = 0;
    GLuint m_nVertexBuffer = 0;
    GLuint m_nIndexBuffer = 0;

    const ShaderUniformHandle* m_pModelUniform = nullptr;
    const ShaderUniformHandle* m_pSpecularParamUniform = nullptr;
    const ShaderUniformHandle* m_pMainTexUniform = nullptr;
    const ShaderUniformHandle* m_pSpecularMapUniform = nullptr;
    const ShaderUniformHandle* m_pNormalMapUniform = nullptr;
    const ShaderUniformHandle* m_pTextureEnabledUniform = nullptr;

    const ShaderUniformHandle* m_pDepthTextureUniform = nullptr;
    const ShaderUniformHandle* m_pLightMatrixUniform1 = nullptr;

    Shader* m_pDepthShader = nullptr;
    const ShaderUniformHandle* m_pDepthModelUniform = nullptr;
    const ShaderUniformHandle* m_pLightMatrixUniform2 = nullptr;


    int m_nIndiceCount = 0;

    void bindVertexArray(Shader* const pShader);
    void bindDepthVertexArray();

    // int m_nIdleAnimationIndex = -1;
    // int m_nWalkAnimationIndex = -1;

    // COMPONENT_REGISTER_SERIALIZABLE(MeshRenderer)
};

// REGISTER_CLASS(MeshRenderer)


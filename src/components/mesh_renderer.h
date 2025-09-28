#include "component.h"

class SimpleObjReader;
class Shader;
class Image;


typedef unsigned int GLuint;


class MeshRenderer : public Component
{
public:
    // Character2d() {};
    MeshRenderer(SimpleObjReader* pMesh);
    ~MeshRenderer();

    virtual bool isIDrawable() const override { return true; }
    virtual bool isUpdatable() const override { return false; }

    virtual void update(float fDeltaTime) override {}

    virtual void draw() override;

    void setShader(Shader* pShader);

private:
    SimpleObjReader* m_pMesh = nullptr;

    Image* m_pMainTexture = nullptr;

    Shader* m_pShader = nullptr;
    GLuint m_nModelUniform = 0;
    GLuint m_nViewUniform = 0;
    GLuint m_nProjectionUniform = 0;

    GLuint m_nVertexArray = 0;
    GLuint m_nVertexBuffer = 0;
    GLuint m_nIndexBuffer = 0;

    int m_nVertexCount = 0;

    void bindTestingVertexArray();
    void bindVertexArray();

    // int m_nIdleAnimationIndex = -1;
    // int m_nWalkAnimationIndex = -1;

    // COMPONENT_REGISTER_SERIALIZABLE(MeshRenderer)
};

// REGISTER_CLASS(MeshRenderer)


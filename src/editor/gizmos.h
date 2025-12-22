#pragma once

#include <vector>
#include "../engine/core/math/vector.h"
#include "../engine/core/math/color.h"
#include "../engine/core/math/quaternion.h"


class Component;
class Shader;
class ShaderUniformHandle;

typedef unsigned int GLuint;


struct ImageGizmosData
{
    Vector3 m_vecPosition;
    Vector3 m_vecColor;
    std::string_view m_strImagePath;
    Component* m_pAttachedComponent = nullptr; // Optional: to link gizmo to a specific component
};

struct SphereGizmosData
{
    Vector3 m_vecPosition;
    float m_fRadius;
    Color m_color;
};

struct CubeGizmosData
{
    Vector3 m_vecPosition;
    Vector3 m_vecSize;
    Color m_color;
};


class GizmosManager
{
public:
    inline static GizmosManager* getInstance() { return ins; }

    static void Initialize()
    {
        if (!ins)
        {
            ins = new GizmosManager();
        }
    }

    void clearAllGizmos();

    void addImageGizmos(Component* const pComponent, const Vector3& vecPosition, const std::string_view& m_strImagePath);
    void addImageGizmos(Component* const pComponent, const Vector3& vecPosition, const std::string_view& m_strImagePath, const Vector3& vecColor);

    void addSphereGizmos(const Vector3& vecPosition, Quaternion rotation, float fRadius);
    void addSphereGizmos(const Vector3& vecPosition, Quaternion rotation, float fRadius, const Color& vecColor);

    void addCubeGizmos(const Vector3& vecPosition, Quaternion rotation, const Vector3& vecSize);
    void addCubeGizmos(const Vector3& vecPosition, Quaternion rotation, const Vector3& vecSize, const Color& vecColor);

    void drawAllGizmos();

private:
    GizmosManager();
    ~GizmosManager();

    static inline GizmosManager* ins = nullptr;

    /* Image Gizmos */
    std::vector<ImageGizmosData> m_vecImageGizmos;
    int m_nImageGizmosSize = 0;

    Shader* m_pImageGizmosShader = nullptr;
    const ShaderUniformHandle* m_pImageGizmosPositionUniform = nullptr;
    const ShaderUniformHandle* m_pImageGizmosColorUniform = nullptr;
    const ShaderUniformHandle* m_pImageGizmosTextureUniform = nullptr;
    const ShaderUniformHandle* m_pImageGizmosUseTextureUniform = nullptr;

    GLuint m_nImageGizmosVertexBuffer = 0;
    GLuint m_nImageGizmosVertexArray = 0;

    void initImageGizmosShaderAndBuffer();
    void drawImageGizmos();


    /* Sphere Gizmos */
    std::vector<SphereGizmosData> m_vecSphereGizmos;
    int m_nSphereGizmosSize = 0;

    Shader* m_pSphereGizmosShader = nullptr;

    GLuint m_nSphereGizmosVertexBuffer = 0;
    GLuint m_nSphereGizmosVertexArray = 0;

    void initSphereGizmosShaderAndBuffer();
    void drawSphereGizmos();


    /* Cube Gizmos */
    std::vector<CubeGizmosData> m_vecCubeGizmos;
    int m_nCubeGizmosSize = 0;

    Shader* m_pCubeGizmosShader = nullptr;
    const ShaderUniformHandle* m_pCubeGizmosPositionUniform = nullptr;
    const ShaderUniformHandle* m_pCubeGizmosSizeUniform = nullptr;
    const ShaderUniformHandle* m_pCubeGizmosColorUniform = nullptr;

    GLuint m_nCubeGizmosVertexBuffer = 0;
    GLuint m_nCubeGizmosVertexArray = 0;

    void initCubeGizmosShaderAndBuffer();
    void drawCubeGizmos();


    void onMouseClickCheck(bool bPressed);
};

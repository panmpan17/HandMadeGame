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

struct CircleGizmosData
{
    Vector3 m_vecPosition;
    float m_fRadius;
    Quaternion m_oRotation;
    Color m_color;
};

struct SphereGizmosData
{
    Vector3 m_vecPosition;
    float m_fRadius;
    Color m_color;
};

struct RectangleGizmosData
{
    Vector3 m_vecPosition;
    Vector2 m_vecSize;
    Quaternion m_oRotation;
    Color m_color;
};

struct CubeGizmosData
{
    Vector3 m_vecPosition;
    Vector3 m_vecSize;
    Quaternion m_oRotation;
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

    void addCircleGizmos(const Vector3& vecPosition, const Quaternion& rotation, float fRadius);
    void addCircleGizmos(const Vector3& vecPosition, const Quaternion& rotation, float fRadius, const Color& vecColor);

    void addSphereGizmos(const Vector3& vecPosition, float fRadius);
    void addSphereGizmos(const Vector3& vecPosition, float fRadius, const Color& vecColor);

    void addRectangleGizmos(const Vector3& vecPosition, const Quaternion& rotation, const Vector2& vecSize);
    void addRectangleGizmos(const Vector3& vecPosition, const Quaternion& rotation, const Vector2& vecSize, const Color& vecColor);

    void addCubeGizmos(const Vector3& vecPosition, const Quaternion& rotation, const Vector3& vecSize);
    void addCubeGizmos(const Vector3& vecPosition, const Quaternion& rotation, const Vector3& vecSize, const Color& vecColor);

    void drawAllGizmos();

private:
    GizmosManager();
    ~GizmosManager();

    static inline GizmosManager* ins = nullptr;

#pragma Image Mesh Gizmos
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
#pragma endregion


#pragma region Mesh Gizmos
    Shader* m_pMeshGizmosShader = nullptr;
    const ShaderUniformHandle* m_pMeshGizmosModelUniform = nullptr;
    const ShaderUniformHandle* m_pMeshGizmosColorUniform = nullptr;


    /* Circle Gizmos */
    std::vector<CircleGizmosData> m_vecCircleGizmos;
    int m_nCircleGizmosSize = 0;
    int m_nCircleGizmosVertexCount = 0;

    GLuint m_nCircleGizmosVertexBuffer = 0;
    GLuint m_nCircleGizmosVertexArray = 0;

    void initCircleGizmosShaderAndBuffer();
    void drawCircleGizmos();


    /* Sphere Gizmos */
    std::vector<SphereGizmosData> m_vecSphereGizmos;
    int m_nSphereGizmosSize = 0;
    int m_nSphereGizmosVertexCount = 0;

    GLuint m_nSphereGizmosVertexBuffer = 0;
    GLuint m_nSphereGizmosVertexArray = 0;

    void initSphereGizmosShaderAndBuffer();
    void drawSphereGizmos();


    /* Rectangle */
    std::vector<RectangleGizmosData> m_vecRectangleGizmos;
    int m_nRectangleGizmosSize = 0;
    int m_nRectangleGizmosVertexCount = 0;

    GLuint m_nRectangleGizmosVertexBuffer = 0;
    GLuint m_nRectangleGizmosVertexArray = 0;

    void initRectangleGizmosShaderAndBuffer();
    void drawRectangleGizmos();


    /* Cube Gizmos */
    std::vector<CubeGizmosData> m_vecCubeGizmos;
    int m_nCubeGizmosSize = 0;
    int m_nCubeGizmosVertexCount = 0;

    GLuint m_nCubeGizmosVertexBuffer = 0;
    GLuint m_nCubeGizmosVertexArray = 0;

    void initCubeGizmosShaderAndBuffer();
    void drawCubeGizmos();
#pragma endregion


    void onMouseClickCheck(bool bPressed);
};

#pragma once

#include <vector>
#include "../engine/core/math/vector.h"
#include "../engine/core/math/color.h"


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

    void addSphereGizmos(const Vector3& vecPosition, float fRadius);
    void addSphereGizmos(const Vector3& vecPosition, float fRadius, const Color& vecColor);

    void addCubeGizmos(const Vector3& vecPosition, const Vector3& vecSize);
    void addCubeGizmos(const Vector3& vecPosition, const Vector3& vecSize, const Color& vecColor);

    void drawAllGizmos();

private:
    GizmosManager();
    ~GizmosManager();

    static inline GizmosManager* ins = nullptr;

    std::vector<ImageGizmosData> m_vecImageGizmos;
    int m_nImageGizmosSize = 0;

    std::vector<SphereGizmosData> m_vecSphereGizmos;
    int m_nSphereGizmosSize = 0;

    std::vector<CubeGizmosData> m_vecCubeGizmos;
    int m_nCubeGizmosSize = 0;

    Shader* m_pImageGizmosShader = nullptr;
    const ShaderUniformHandle* m_pPositionUniform = nullptr;
    const ShaderUniformHandle* m_pColorUniform = nullptr;
    const ShaderUniformHandle* m_pTextureUniform = nullptr;
    const ShaderUniformHandle* m_pUseTextureUniform = nullptr;

    GLuint m_nVertexBuffer = 0;
    GLuint m_nVertexArray = 0;

    void registerBuffer();

    void drawSphereGizmos();
    void drawCubeGizmos();
    void drawImageGizmos();

    void onMouseClickCheck(bool bPressed);
};

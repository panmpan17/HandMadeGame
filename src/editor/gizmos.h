#pragma once

#include <vector>
#include "../engine/core/math/vector.h"


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
    void addGizmos(Component* const pComponent, const Vector3& vecPosition, const std::string_view& m_strImagePath);
    void addGizmos(Component* const pComponent, const Vector3& vecPosition, const std::string_view& m_strImagePath, const Vector3& vecColor);

    void drawAllGizmos();

private:
    GizmosManager();
    ~GizmosManager();

    static inline GizmosManager* ins = nullptr;

    std::vector<ImageGizmosData> m_vecImageGizmos;
    int m_nImageGizmosSize = 0;

    Shader* m_pImageGizmosShader = nullptr;
    const ShaderUniformHandle* m_pPositionUniform = nullptr;
    const ShaderUniformHandle* m_pColorUniform = nullptr;
    const ShaderUniformHandle* m_pTextureUniform = nullptr;
    const ShaderUniformHandle* m_pUseTextureUniform = nullptr;

    GLuint m_nVertexBuffer = 0;
    GLuint m_nVertexArray = 0;

    void registerBuffer();

    void onMouseClickCheck(bool bPressed);
};

#pragma once

#include <vector>
#include "../engine/core/math/vector.h"


class Shader;
class ShaderUniformHandle;

typedef unsigned int GLuint;


struct ImageGizmosData
{
    Vector3 m_vecPosition;
    Vector3 m_vecColor;
    float m_fScale;
    std::string_view m_strImagePath;
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
    void addGizmos(const Vector3& vecPosition, const std::string_view& m_strImagePath, float fScale = 1);
    void addGizmos(const Vector3& vecPosition, const std::string_view& m_strImagePath, float fScale, const Vector3& vecColor);

    void drawAllGizmos();

private:
    GizmosManager() { registerBuffer(); }

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
};

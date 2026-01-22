#pragma once

// #include "../expandable_array.h"
#include <unordered_map>
#include <string>
#include "../../utils/file_watch_dog.h"
#include "shader.h"

#define GL_INVALID_INDEX 0xFFFFFFFF

struct ShaderRegisteryData
{
    int nCurrentShaderId = -1;
    std::string m_strName;
    bool m_bTransparent = false;

    // OpenGL
    std::string m_strVertexPath;
    std::string m_strFragmentPath;
    GLuint nCameraUBOIndex = GL_INVALID_INDEX;
    GLuint nLightUBOIndex = GL_INVALID_INDEX;
    GLuint nTimeDataUBOIndex = GL_INVALID_INDEX;

    // Metal
    std::string m_strMetalShaderPrefix;
    std::vector<int> m_metalAttributeSizes;
    bool m_bMetalAttributePack = false;

    void reset()
    {
        nCurrentShaderId = -1;
        m_strName.clear();
        m_bTransparent = false;

        m_strVertexPath.clear();
        m_strFragmentPath.clear();
        nCameraUBOIndex = GL_INVALID_INDEX;
        nLightUBOIndex = GL_INVALID_INDEX;
        nTimeDataUBOIndex = GL_INVALID_INDEX;

        m_strMetalShaderPrefix.clear();
        m_metalAttributeSizes.clear();
    }
};


class ShaderLoader
{
public:
    inline static ShaderLoader* getInstance() { return ins; }

    inline static void Initialize() { if (!ins) ins = new ShaderLoader(); }
    static void Cleanup() { if (ins) { delete ins; ins = nullptr; } }

    Shader* getShader(int nId) const;
    Shader* getShader(const std::string& strName) const;

    void reloadAllShaders();

    void onFileChangedListener(const std::string& strFilePath, eFileChangeType eType);

    void onMainCameraChanged();

private:
    static ShaderLoader* ins;

    ShaderLoader();
    ~ShaderLoader();

    void readRegistryFromFile();

    // PointerExpandableArray<Shader*> shaders = (5);
    std::unordered_map<int, Shader*> m_mapShaders;
};

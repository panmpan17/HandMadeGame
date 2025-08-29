#pragma once

// #include "../expandable_array.h"
#include <unordered_map>
#include <string>


class Shader;


class ShaderLoader
{
public:
    inline static ShaderLoader* getInstance() { return ins; }

    inline static void Initialize() { if (!ins) ins = new ShaderLoader(); }

    Shader* getShader(int nId) const;
    Shader* getShader(const std::string_view& strName) const;

private:
    static ShaderLoader* ins;

    ShaderLoader();
    ~ShaderLoader();

    void readRegistryFromFile();

    // PointerExpandableArray<Shader*> shaders = (5);
    std::unordered_map<int, Shader*> m_mapShaders;
};

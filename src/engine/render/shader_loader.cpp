#include "shader_loader.h"
#include "shader.h"
#include "../core/debug_macro.h"
#include "../../utils/file_utils.h"
#include "../../utils/platform.h"

#if IS_PLATFORM_MACOS
#elif IS_PLATFORM_WINDOWS
#include <cstring>
#endif

constexpr std::string_view SHADER_REGISTRY_FILE = "assets/shaders/shader_registry.yaml";


ShaderLoader* ShaderLoader::ins = nullptr;

ShaderLoader::ShaderLoader()
{
    ins = this;

    readRegistryFromFile();
}

ShaderLoader::~ShaderLoader()
{
    for (auto& pair : m_mapShaders)
    {
        delete pair.second;
    }
}

void ShaderLoader::readRegistryFromFile()
{
    auto reader = FileReader(SHADER_REGISTRY_FILE);

    if (!reader.isOpen())
    {
        throw std::runtime_error("Failed to open shader registry file: " + std::string(SHADER_REGISTRY_FILE));
    }

    int nCurrentShaderId = -1;
    std::string strCurrentShaderName, strCurrentVertexPath, strCurrentFragmentPath;

    // Read shader paths from the registry file
    std::string strLine;
    while (reader.readLine(strLine))
    {
        if (strLine.front() == '#')
        {
            continue;
        }

        if (strLine.front() != ' ')
        {
            if (nCurrentShaderId != -1 && !strCurrentShaderName.empty() && !strCurrentVertexPath.empty() && !strCurrentFragmentPath.empty())
            {
                auto pShader = new Shader(nCurrentShaderId, strCurrentShaderName, strCurrentVertexPath, strCurrentFragmentPath);
                m_mapShaders[nCurrentShaderId] = pShader;
                strCurrentShaderName = "";
                strCurrentVertexPath = "";
                strCurrentFragmentPath = "";
            }

            // Id of the shader
            nCurrentShaderId = std::stoi(strLine.substr(0, strLine.length() - 1));
        }
        else if (memcmp(strLine.data() + 2, "name", 4) == 0)
        {
            strCurrentShaderName = strLine.substr(2 + 6);
        }
        else if (memcmp(strLine.data() + 2, "vertex", 6) == 0)
        {
            strCurrentVertexPath = strLine.substr(2 + 8);
        }
        else if (memcmp(strLine.data() + 2, "fragment", 8) == 0)
        {
            strCurrentFragmentPath = strLine.substr(2 + 10);
        }
    }

    if (nCurrentShaderId != -1)
    {
        m_mapShaders[nCurrentShaderId] = new Shader(nCurrentShaderId, strCurrentShaderName, strCurrentVertexPath, strCurrentFragmentPath);
    }
}

Shader* ShaderLoader::getShader(int nId) const
{
    auto iterFind = m_mapShaders.find(nId);
    if (iterFind != m_mapShaders.end())
    {
        return iterFind->second;
    }
    return nullptr;
}

Shader* ShaderLoader::getShader(const std::string_view& strName) const
{
    for (const auto& pair : m_mapShaders)
    {
        if (pair.second->getName() == strName)
        {
            return pair.second;
        }
    }
    return nullptr;
}

void ShaderLoader::reloadAllShaders()
{
    for (auto& pair : m_mapShaders)
    {
        pair.second->reload();
    }
}

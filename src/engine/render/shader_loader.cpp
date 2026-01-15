#include "shader_loader.h"
#include "shader.h"
#include "../core/debug_macro.h"
#include "../../utils/file_utils.h"
#include "../../utils/platform.h"

#if IS_PLATFORM_MACOS
#elif IS_PLATFORM_WINDOWS
#include <cstring>
#endif
#include <functional>

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

    ShaderRegisteryData oCurrentShaderData;

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
            if (oCurrentShaderData.nCurrentShaderId != -1 && !oCurrentShaderData.m_strName.empty() && !oCurrentShaderData.m_strVertexPath.empty() && !oCurrentShaderData.m_strFragmentPath.empty())
            {
                m_mapShaders.insert({oCurrentShaderData.nCurrentShaderId, Shader::loadFromOpenGLShader(oCurrentShaderData)});
                oCurrentShaderData.reset();
            }

            // Id of the shader
            oCurrentShaderData.nCurrentShaderId = std::stoi(strLine.substr(0, strLine.length() - 1));
        }
        else if (memcmp(strLine.data() + 2, "name", 4) == 0)
        {
            oCurrentShaderData.m_strName = strLine.substr(2 + 6);
        }
        else if (memcmp(strLine.data() + 2, "vertex", 6) == 0)
        {
            oCurrentShaderData.m_strVertexPath = strLine.substr(2 + 8);
        }
        else if (memcmp(strLine.data() + 2, "fragment", 8) == 0)
        {
            oCurrentShaderData.m_strFragmentPath = strLine.substr(2 + 10);
        }
        else if (memcmp(strLine.data() + 2, "cameraUBO", 9) == 0)
        {
            oCurrentShaderData.nCameraUBOIndex = std::stoi(strLine.substr(2 + 11));
        }
        else if (memcmp(strLine.data() + 2, "lightUBO", 8) == 0)
        {
            oCurrentShaderData.nLightUBOIndex = std::stoi(strLine.substr(2 + 10));
        }
        else if (memcmp(strLine.data() + 2, "timeUBO", 7) == 0)
        {
            oCurrentShaderData.nTimeDataUBOIndex = std::stoi(strLine.substr(2 + 9));
        }
    }

    if (oCurrentShaderData.nCurrentShaderId != -1)
    {
        m_mapShaders.insert({oCurrentShaderData.nCurrentShaderId, Shader::loadFromOpenGLShader(oCurrentShaderData)});
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

void ShaderLoader::onFileChangedListener(const std::string& strFilePath, eFileChangeType eType)
{
    if (eType != eFileChangeType::FILE_MODIFIED)
    {
        return;
    }

    for (auto& pair : m_mapShaders)
    {
        if (pair.second->getIsUsingFile(strFilePath))
        {
            pair.second->reload();
        }
    }
}

void ShaderLoader::onMainCameraChanged()
{
    for (auto& pair : m_mapShaders)
    {
        pair.second->reloadCameraUBOBinding();
    }
}

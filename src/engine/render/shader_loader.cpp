#include "shader_loader.h"
#include "shader.h"
#include "../core/debug_macro.h"
#include "../../utils/file_utils.h"
#include "../../utils/platform.h"
#include "../core/window.h"

#if __APPLE__

#include "../core/metal/helper.h"

#elif IS_PLATFORM_WINDOWS
#include <cstring>
#endif
#include <functional>

constexpr std::string_view SHADER_REGISTRY_FILE = "assets/shaders/shader_registry.yaml";
constexpr std::string_view SHADER_METAL_LIB_FILE = "assets/metal_shaders.metallib";


#define GL_INVALID_INDEX 0xFFFFFFFF


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
    GLuint nCameraUBOIndex = GL_INVALID_INDEX;
    GLuint nLightUBOIndex = GL_INVALID_INDEX;
    GLuint nTimeDataUBOIndex = GL_INVALID_INDEX;
    std::string strCurrentShaderName, strCurrentVertexPath, strCurrentFragmentPath;
    std::string strMetalShaderPrefix;

#if __APPLE__
    std::function<void()> funcCreateMetalShader = nullptr;

    if (Window::ins->isUsingMetal())
    {
        MTL::Device* pMetalDevice = Window::ins->getMetalDevice();
        MTL::Library* pLibrary = nullptr;

        pLibrary = loadLibraryFromPath(pMetalDevice, SHADER_METAL_LIB_FILE.data());
        funcCreateMetalShader = [this, &nCurrentShaderId, &strCurrentShaderName, &strMetalShaderPrefix, &pLibrary, &pMetalDevice]()
        {
            auto pShader = Shader::loadFromMetalShader(
                pLibrary,
                pMetalDevice,
                nCurrentShaderId,
                strCurrentShaderName,
                strMetalShaderPrefix);

            if (!pShader)
            {
                LOGLN("Failed to load Metal shader: {} with prefix: {}", strCurrentShaderName, strMetalShaderPrefix);
                return;
            }

            m_mapShaders.insert({ nCurrentShaderId, pShader });
        };
    }
#endif // __APPLE__

    std::function funcCreateOpenGLShader = [this, &nCurrentShaderId, &nCameraUBOIndex, &nLightUBOIndex, &nTimeDataUBOIndex, &strCurrentShaderName, &strCurrentVertexPath, &strCurrentFragmentPath]()
    {
        auto pShader = new Shader(nCurrentShaderId, strCurrentShaderName, strCurrentVertexPath, strCurrentFragmentPath);
        if (nCameraUBOIndex != GL_INVALID_INDEX)
        {
            pShader->setCameraUBOBindingPoint(nCameraUBOIndex);
        }
        if (nLightUBOIndex != GL_INVALID_INDEX)
        {
            pShader->setLightUBOBindingPoint(nLightUBOIndex);
        }
        if (nTimeDataUBOIndex != GL_INVALID_INDEX)
        {
            pShader->setTimeDataUBOBindingPoint(nTimeDataUBOIndex);
        }
        m_mapShaders.insert({ nCurrentShaderId, pShader });
    };

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
            if (nCurrentShaderId != -1 && !strCurrentShaderName.empty())
            {
#if __APPLE__
                if (funcCreateMetalShader)
                {
                    if (!strMetalShaderPrefix.empty())
                    {
                        funcCreateMetalShader();
                    }
                }
                else
#endif
                if (!strCurrentVertexPath.empty() && !strCurrentFragmentPath.empty())
                {
                    funcCreateOpenGLShader();
                }

                strCurrentShaderName = "";
                strCurrentVertexPath = "";
                strCurrentFragmentPath = "";
                nCameraUBOIndex = GL_INVALID_INDEX;
                nLightUBOIndex = GL_INVALID_INDEX;
                nTimeDataUBOIndex = GL_INVALID_INDEX;
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
        else if (memcmp(strLine.data() + 2, "cameraUBO", 9) == 0)
        {
            nCameraUBOIndex = std::stoi(strLine.substr(2 + 11));
        }
        else if (memcmp(strLine.data() + 2, "lightUBO", 8) == 0)
        {
            nLightUBOIndex = std::stoi(strLine.substr(2 + 10));
        }
        else if (memcmp(strLine.data() + 2, "timeUBO", 7) == 0)
        {
            nTimeDataUBOIndex = std::stoi(strLine.substr(2 + 9));
        }
        else if (memcmp(strLine.data() + 2, "metal_prefix", 12) == 0)
        {
            strMetalShaderPrefix = strLine.substr(2 + 14);
        }
    }

    if (nCurrentShaderId != -1 && !strCurrentShaderName.empty())
    {
#if __APPLE__
        if (funcCreateMetalShader)
        {
            if (!strMetalShaderPrefix.empty())
            {
                funcCreateMetalShader();
            }
        }
        else
#endif
        if (!strCurrentVertexPath.empty() && !strCurrentFragmentPath.empty())
        {
            funcCreateOpenGLShader();
        }
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

Shader* ShaderLoader::getShader(const std::string& strName) const
{
    for (const auto& pair : m_mapShaders)
    {
        // LOGLN("Checking shader: {}, {}", pair.second->getName(), strName);
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

#include "shader_loader.h"
#include "shader.h"
#include "core/renderer.h"
#include "../core/debug_macro.h"
#include "../../utils/file_utils.h"
#include "../../utils/platform.h"
#include "../../utils/string_handle.h"
#include "../core/window.h"

#if __APPLE__

#include "../core/metal/helper.h"

#elif IS_PLATFORM_WINDOWS
#include <cstring>
#endif
#include <functional>

constexpr std::string_view SHADER_REGISTRY_FILE = "assets/shaders/shader_registry.yaml";
constexpr std::string_view SHADER_METAL_LIB_FILE = "assets/metal_shaders.metallib";


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

#if __APPLE__
    std::function<void()> funcCreateMetalShader = nullptr;

    if (Renderer::isUsingMetal())
    {
        MTL::Device* pMetalDevice = Window::ins->getMetalDevice();
        MTL::Library* pLibrary = nullptr;

        pLibrary = loadLibraryFromPath(pMetalDevice, SHADER_METAL_LIB_FILE.data());

#if IS_DEBUG_VERSION
        NS::Array* pFunctionNames = pLibrary->functionNames();
        int count = static_cast<int>(pFunctionNames->count());
        for (int i = 0; i < count; ++i)
        {
            NS::String* pFuncName = static_cast<NS::String*>(pFunctionNames->object(i));
            LOGLN("Metal Library Function {}: {}", i, pFuncName->utf8String());
        }
#endif

        funcCreateMetalShader = [this, &oCurrentShaderData, &pLibrary, &pMetalDevice]()
        {
            auto pShader = Shader::loadFromMetalShader(
                pLibrary,
                pMetalDevice,
                oCurrentShaderData);

            if (!pShader)
            {
                LOGLN("Failed to load Metal shader: {} with prefix: {}", oCurrentShaderData.m_strName, oCurrentShaderData.m_strMetalShaderPrefix);
                return;
            }

            m_mapShaders.insert({ oCurrentShaderData.nCurrentShaderId, pShader });
        };
    }
#endif // __APPLE__

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
            if (oCurrentShaderData.nCurrentShaderId != -1 && !oCurrentShaderData.m_strName.empty())
            {
                if (Renderer::isUsingOpenGL())
                {
                    if (!oCurrentShaderData.m_strVertexPath.empty() && !oCurrentShaderData.m_strFragmentPath.empty())
                    {
                        m_mapShaders.insert({oCurrentShaderData.nCurrentShaderId, Shader::loadFromOpenGLShader(oCurrentShaderData)});
                    }
                }
#if __APPLE__
                else if (Renderer::isUsingMetal())
                {
                    if (!oCurrentShaderData.m_strMetalShaderPrefix.empty())
                    {
                        funcCreateMetalShader();
                    }
                }
#endif
            }

            oCurrentShaderData.reset();

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
        else if (memcmp(strLine.data() + 2, "metal_prefix", 12) == 0)
        {
            oCurrentShaderData.m_strMetalShaderPrefix = strLine.substr(2 + 14);
        }
        else if (memcmp(strLine.data() + 2, "metal_attribute_size", 20) == 0)
        {
            std::string strSizes = strLine.substr(2 + 22);
            oCurrentShaderData.m_metalAttributeSizes.clear();
            splitStringPush(oCurrentShaderData.m_metalAttributeSizes, strSizes, ',', true);
        }
        else if (memcmp(strLine.data() + 2, "transparency", 12) == 0)
        {
            oCurrentShaderData.m_bTransparent = (strLine.substr(2 + 14) == "1");
        }
        else if (memcmp(strLine.data() + 2, "metal_attribute_pack", 20) == 0)
        {
            oCurrentShaderData.m_bMetalAttributePack = (strLine.substr(2 + 22) == "1");
        }
        else if (memcmp(strLine.data() + 2, "metal_vertex_func", 17) == 0)
        {
            oCurrentShaderData.m_strMetalVertexShaderFunc = strLine.substr(2 + 19);
        }
        else if (memcmp(strLine.data() + 2, "metal_fragment_func", 19) == 0)
        {
            oCurrentShaderData.m_strMetalFragmentShaderFunc = strLine.substr(2 + 21);
        }
    }

    if (oCurrentShaderData.nCurrentShaderId != -1 && !oCurrentShaderData.m_strName.empty())
    {
#if __APPLE__
        if (funcCreateMetalShader)
        {
            if (!oCurrentShaderData.m_strMetalShaderPrefix.empty())
            {
                funcCreateMetalShader();
            }
        }
        else
#endif
        if (!oCurrentShaderData.m_strVertexPath.empty() && !oCurrentShaderData.m_strFragmentPath.empty())
        {
            m_mapShaders.insert({oCurrentShaderData.nCurrentShaderId, Shader::loadFromOpenGLShader(oCurrentShaderData)});
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

    if (Renderer::isUsingMetal())
    {
        // NOTE: Because metal shader require precompilation, which is might need extra tool to trigger recompilation.
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

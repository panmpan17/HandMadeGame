#include "material_loader.h"

#include "shader_loader.h"
#include "image_loader.h"
#include "../core/debug_macro.h"
#include "../../utils/file_utils.h"
#include "../../utils/platform.h"


MaterialLoader::MaterialLoader()
{
    if (ShaderLoader::getInstance() == nullptr)
    {
        throw std::runtime_error("ShaderLoader must be initialized before MaterialLoader.");
    }
}

MaterialLoader::~MaterialLoader()
{
}

std::shared_ptr<Material> MaterialLoader::getMaterial(const std::string& strFilePath)
{
    auto it = m_mapMaterials.find(strFilePath);
    if (it != m_mapMaterials.end())
    {
        return it->second;
    }

    return loadMaterialFromFile(strFilePath);
}

enum class MaterialUniformType : uint8_t
{
    None,
    Texture2D,
};

std::shared_ptr<Material> MaterialLoader::loadMaterialFromFile(const std::string& strFilePath)
{
    auto reader = FileReader(strFilePath);
    if (!reader.isOpen())
    {
        LOGLN("Failed to open shader registry file: {}", strFilePath);
        return nullptr;
    }

    std::shared_ptr<Material> pMaterial = nullptr;

    MaterialUniformType eUniformType = MaterialUniformType::None;
    std::string strLine;
    while (reader.readLine(strLine))
    {
        if (memcmp(strLine.data(), "shader", 6) == 0)
        {
            if (pMaterial)
            {
                LOGLN("Material file {} error: multiple shader definitions found.", strFilePath);
                return nullptr;
            }

            std::string strShaderName = strLine.substr(2 + 6);
            Shader* pShader = ShaderLoader::getInstance()->getShader(strShaderName);
            if (!pShader)
            {
                LOGLN("Material file {} error: shader {} not found.", strFilePath, strShaderName);
                return nullptr;
            }

            pMaterial = std::make_shared<Material>(pShader);
        }
        else if (memcmp(strLine.data(), "texture", 7) == 0)
        {
            if (!pMaterial)
            {
                LOGLN("Material file {} error: texture defined before shader.", strFilePath);
                return nullptr;
            }

            eUniformType = MaterialUniformType::Texture2D;
        }
        else if (memcmp(strLine.data(), "  ", 2) == 0)
        {
            if (eUniformType == MaterialUniformType::Texture2D)
            {
                size_t nSeperate = strLine.find(": ");
                if (nSeperate == std::string::npos)
                {
                    LOGLN("Material file {} error: invalid texture definition.", strFilePath);
                    return nullptr;
                }

                std::string strUniformName = strLine.substr(2, nSeperate - 2);
                std::string strImagePath = strLine.substr(nSeperate + 2);

                Image* pImage = ImageLoader::getInstance()->getImageByPath(strImagePath);
                if (!pImage)
                {
                    LOGLN("Material file {} error: image {} not found.", strFilePath, strImagePath);
                    return nullptr;
                }

                pMaterial->bindTextureWithImage(strUniformName, pImage);
            }
        }
    }

    if (pMaterial)
    {
        m_mapMaterials[strFilePath] = pMaterial;
    }

    return pMaterial;
}

#include "material.h"

#include <glad/gl.h>
#include "shader.h"
#include "image.h"
#include "../core/debug_macro.h"


void Material::useShader() const
{
    if (m_pShader)
    {
        glUseProgram(m_pShader->getProgram());
    }
}

void Material::bindTextureWithImage(const std::string_view& strUniformName, Image* pImage)
{
    if (!m_pShader)
    {
        return;
    }

    TextureUniform texUniform;
    texUniform.pUniformHandle = m_pShader->getUniformHandle(strUniformName);
    if (!texUniform.pUniformHandle)
    {
        return;
    }

    texUniform.pImage = pImage;
    m_vecTextureUniforms.push_back(texUniform);
}

Image* Material::getImageByUniformName(const std::string& strUniformName) const
{
    int nSize = static_cast<int>(m_vecTextureUniforms.size());
    for (int i = 0; i < nSize; ++i)
    {
        const TextureUniform& texUniform = m_vecTextureUniforms[i];
        if (texUniform.pUniformHandle && texUniform.pUniformHandle->m_strName == strUniformName)
        {
            return texUniform.pImage;
        }
    }
    return nullptr;
}
Image* Material::getImageByUniformName(const std::string_view& strUniformName) const
{
    int nSize = static_cast<int>(m_vecTextureUniforms.size());
    for (int i = 0; i < nSize; ++i)
    {
        const TextureUniform& texUniform = m_vecTextureUniforms[i];
        if (texUniform.pUniformHandle)
        {
            if (texUniform.pUniformHandle->m_strName == strUniformName)
            {
                return texUniform.pImage;
            }
        }
    }
    return nullptr;
}

int Material::sendTexturesData() const
{
    int nTextureBitmask = 0;

    int nSize = static_cast<int>(m_vecTextureUniforms.size());
    for (int nTextureIndex = 0; nTextureIndex < nSize; ++nTextureIndex)
    {
        const TextureUniform& texUniform = m_vecTextureUniforms[nTextureIndex];
        if (ShaderUniformHandle::sendTexture(texUniform.pUniformHandle, texUniform.pImage, nTextureIndex))
        {
            nTextureBitmask |= (1 << nTextureIndex); // Enable corresponding texture
        }
    }

    return nTextureBitmask;
}

void Material::syncTo(const Material* const pOtherMaterial)
{
    if (!pOtherMaterial)
    {
        return;
    }

    m_pShader = pOtherMaterial->m_pShader;
    m_vecTextureUniforms = pOtherMaterial->m_vecTextureUniforms;
}

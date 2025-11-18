#include "material.h"

#include <glad/gl.h>
#include "shader.h"


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

int Material::sendTexturesData() const
{
    int nTextureBitmask = 0;

    for (int nTextureIndex = 0; nTextureIndex < static_cast<int>(m_vecTextureUniforms.size()); ++nTextureIndex)
    {
        const TextureUniform& texUniform = m_vecTextureUniforms[nTextureIndex];
        if (ShaderUniformHandle::sendTexture(texUniform.pUniformHandle, texUniform.pImage, nTextureIndex))
        {
            nTextureBitmask |= (1 << nTextureIndex); // Enable corresponding texture
        }
    }

    return nTextureBitmask;
}
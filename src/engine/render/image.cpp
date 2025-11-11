#include "image.h"
#include <stb_image.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>

#include "../../utils/filesystem.h"
#include "../../utils/file_utils.h"
#include "../core/debug_macro.h"


Image::Image(const std::string& strPath, bool flipVertically/* = true */)
{
    stbi_set_flip_vertically_on_load(flipVertically);

    m_strPath = strPath;

    if (*strPath.begin() != '/')
    {
        std::string strFullPath = fs::path(FileUtils::getResourcesPath()).append(strPath).string();

        m_pData = stbi_load(strFullPath.c_str(), &m_nWidth, &m_nHeight, &m_nChannels, 0);
        if (!m_pData)
        {
            LOGERR("Failed to load image: {}", strFullPath);
        }
    }
    else
    {
        m_pData = stbi_load(strPath.c_str(), &m_nWidth, &m_nHeight, &m_nChannels, 0);
        if (!m_pData)
        {
            LOGERR("Failed to load image: {}", strPath);
        }
    }
}

Image::Image(const std::string_view& strPath, bool flipVertically/* = true */)
{
    stbi_set_flip_vertically_on_load(flipVertically);

    m_strPath = strPath;

    if (*strPath.begin() != '/')
    {
        std::string strFullPath = fs::path(FileUtils::getResourcesPath()).append(strPath).string();

        m_pData = stbi_load(strFullPath.c_str(), &m_nWidth, &m_nHeight, &m_nChannels, 0);
        if (!m_pData)
        {
            LOGERR("Failed to load image: {}", strFullPath);
        }
    }
    else
    {
        m_pData = stbi_load(strPath.data(), &m_nWidth, &m_nHeight, &m_nChannels, 0);
        if (!m_pData)
        {
            LOGERR("Failed to load image: {}", strPath);
        }
    }

}

Image::Image(const aiTexture* pAiTexture, bool flipVertically/* = true */)
{
    stbi_set_flip_vertically_on_load(flipVertically);

    m_strPath = "<embedded_texture>";

    if (pAiTexture->mHeight == 0)
    {
        // Compressed texture
        m_pData = stbi_load_from_memory(
            reinterpret_cast<const stbi_uc*>(pAiTexture->pcData),
            pAiTexture->mWidth,
            &m_nWidth,
            &m_nHeight,
            &m_nChannels,
            0
        );
        if (!m_pData)
        {
            LOGERR("Failed to load embedded compressed texture");
        }
    }
    else
    {
        // Uncompressed texture
        m_nWidth = pAiTexture->mWidth;
        m_nHeight = pAiTexture->mHeight;
        m_nChannels = 4; // Assimp stores uncompressed textures as ARGB8888
        size_t nDataSize = m_nWidth * m_nHeight * m_nChannels;
        m_pData = new unsigned char[nDataSize];
        memcpy(m_pData, pAiTexture->pcData, nDataSize);
    }
}

Image::~Image()
{
    freeCPUData();
}

void Image::loadTextureToGL()
{
    glGenTextures(1, &m_nTextureID);
    glBindTexture(GL_TEXTURE_2D, m_nTextureID);
    
    // GL_REPEAT: Repeats the texture image
    // GL_CLAMP_TO_EDGE: Clamps to the edge pixel, good for avoiding bleeding on spritesheets
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // GL_LINEAR: Smooth filtering
    // GL_NEAREST: Pixelated filtering (good for pixel art)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (m_nChannels == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_nWidth, m_nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pData);
    }
    else if (m_nChannels == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nWidth, m_nHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pData);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
}

void Image::freeCPUData()
{
    if (m_pData)
    {
        stbi_image_free(m_pData);
        m_pData = nullptr;
    }
}

#include "image.h"
#include <stb_image.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>

#include "../core/window.h"

#include "../../utils/filesystem.h"
#include "../../utils/file_utils.h"
#include "../core/debug_macro.h"


Image::Image(const std::string& strPath, bool flipVertically/* = true */) : m_strPath(strPath)
{
    stbi_set_flip_vertically_on_load(flipVertically);
    configureAndLoadToGPU();
}

Image::Image(const std::string_view& strPath, bool flipVertically/* = true */) : m_strPath(std::string(strPath))
{
    stbi_set_flip_vertically_on_load(flipVertically);
    configureAndLoadToGPU();
}

void Image::configureAndLoadToGPU()
{
    int nForcedChannels = 0;

#if __APPLE__
    if (Window::ins->isUsingMetal())
    {
        if (memcmp(m_strPath.c_str() + m_strPath.size() - 4, ".jpg", 4) == 0
            || memcmp(m_strPath.c_str() + m_strPath.size() - 5, ".jpeg", 5) == 0)
        {
            nForcedChannels = 4;
        }
    }
#endif

    loadFileToGPU(nForcedChannels);

#if __APPLE__
    if (Window::ins->isUsingMetal() && (m_nChannels == 3 && nForcedChannels != 4))
    {
        freeCPUData();
        nForcedChannels = 4;
        loadFileToGPU(nForcedChannels);
    }
#endif
}

void Image::loadFileToGPU(int nDesiredChannels)
{
    if (*m_strPath.begin() != '/')
    {
        std::string strFullPath = fs::path(FileUtils::getResourcesPath()).append(m_strPath).string();

        m_pData = stbi_load(strFullPath.c_str(), &m_nWidth, &m_nHeight, &m_nChannels, nDesiredChannels);
        if (!m_pData)
        {
            LOGERR("Failed to load image: {}", strFullPath);
        }
    }
    else
    {
        m_pData = stbi_load(m_strPath.c_str(), &m_nWidth, &m_nHeight, &m_nChannels, nDesiredChannels);
        if (!m_pData)
        {
            LOGERR("Failed to load image: {}", m_strPath);
        }
    }

    if (m_nChannels != nDesiredChannels && nDesiredChannels != 0)
    {
        m_nChannels = nDesiredChannels;
    }
}

Image::Image(const aiTexture* pAiTexture, const char* strName, bool flipVertically/* = true */)
{
    stbi_set_flip_vertically_on_load(flipVertically);

    m_strPath = strName ? strName : "<embedded_texture>";

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

Image::Image(int nWidth, int nHeight, int nChannels, unsigned char* pData)
    : m_nWidth(nWidth), m_nHeight(nHeight), m_nChannels(nChannels), m_pData(pData)
{
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
    else if (m_nChannels == 2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_nWidth, m_nHeight, 0, GL_RG, GL_UNSIGNED_BYTE, m_pData);
    }
    else if (m_nChannels == 1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nWidth, m_nHeight, 0, GL_RED, GL_UNSIGNED_BYTE, m_pData);
    }
    else
    {
        LOGERR("Unsupported number of channels ({}) in image: {}", m_nChannels, m_strPath);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
}

#if __APPLE__
void Image::loadTextureToMetal()
{
    MTL::PixelFormat pixelFormat = MTL::PixelFormatInvalid;

    switch (m_nChannels)
    {
        case 4:
            pixelFormat = MTL::PixelFormat::PixelFormatRGBA8Unorm;
            break;
        case 3:
            LOGERR("Metal does not support 3-channel textures directly. Please convert to 4 channels.: {}", m_strPath);
            return;
        case 2:
            pixelFormat = MTL::PixelFormat::PixelFormatRG8Unorm;
            break;
        case 1:
            pixelFormat = MTL::PixelFormat::PixelFormatR8Unorm;
            break;
        default:
            LOGERR("Unsupported number of channels ({}) in image: {}", m_nChannels, m_strPath);
            return;
    }

    MTL::TextureDescriptor* pTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
    pTextureDescriptor->setPixelFormat(pixelFormat);
    pTextureDescriptor->setWidth(static_cast<NS::UInteger>(m_nWidth));
    pTextureDescriptor->setHeight(static_cast<NS::UInteger>(m_nHeight));
    pTextureDescriptor->setUsage(MTL::TextureUsageShaderRead);

    m_pMetalTexture = Window::ins->getMetalDevice()->newTexture(pTextureDescriptor);

    MTL::Region region = MTL::Region::Make2D(0, 0, static_cast<NS::UInteger>(m_nWidth), static_cast<NS::UInteger>(m_nHeight));
    m_pMetalTexture->replaceRegion(region, 0, m_pData, static_cast<NS::UInteger>(m_nWidth * m_nChannels));

    pTextureDescriptor->release();
}
#endif

void Image::freeCPUData()
{
    if (m_pData)
    {
        stbi_image_free(m_pData);
        m_pData = nullptr;
    }
}

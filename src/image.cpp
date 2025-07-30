#include "image.h"
#include <stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "debug_macro.h"

Image::Image(const std::string& strPath)
{
    m_pData = stbi_load(strPath.c_str(), &m_nWidth, &m_nHeight, &m_nChannels, 0);
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

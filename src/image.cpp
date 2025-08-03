#include "image.h"
#include <stb_image.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "file_utils.h"
#include "debug_macro.h"

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

Image::Image(const std::string& strPath)
{
    stbi_set_flip_vertically_on_load(true);

    std::string fullPath = strPath;
    if (*strPath.begin() != '/')
    {
        std::string executablePath = FileUtils::getExecutablePath();
        fullPath = fs::path(executablePath).parent_path().append(strPath).string();
    }

    m_pData = stbi_load(fullPath.c_str(), &m_nWidth, &m_nHeight, &m_nChannels, 0);

    if (!m_pData)
    {
        LOGERR_EX("Failed to load image: {}", fullPath);
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

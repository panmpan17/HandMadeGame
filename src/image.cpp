#include "image.h"
#include <stb_image.h>

Image::Image(const std::string& strPath)
{
    m_pData = stbi_load(strPath.c_str(), &m_nWidth, &m_nHeight, &m_nChannels, 0);
}

Image::~Image()
{
    if (m_pData)
    {
        stbi_image_free(m_pData);
    }
}

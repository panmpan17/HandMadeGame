#include "image_loader.h"

void ImageLoader::Initialize()
{
    if (!ins)
    {
        ins = new ImageLoader();
    }
}

void ImageLoader::Cleanup()
{
    if (ins)
    {
        delete ins;
        ins = nullptr;
    }
}


ImageLoader::ImageLoader()
{

}

ImageLoader::~ImageLoader()
{
    for (auto& pair : m_mapLoadedImages)
    {
        delete pair.second; // Free the Image object
    }
    m_mapLoadedImages.clear();
}

void ImageLoader::registerImage(const std::string_view& strName, const std::string_view& strPath)
{
    auto iterFind = m_mapLoadedImages.find(strName);
    if (iterFind == m_mapLoadedImages.end())
    {
        auto pImage = new Image(strPath);
        if (pImage->isCPULoaded())
        {
            pImage->loadTextureToGL();
            pImage->freeCPUData();
        }
        m_mapLoadedImages[strName] = pImage;
    }
}

Image* ImageLoader::getImage(const std::string_view& strName)
{
    auto iterFind = m_mapLoadedImages.find(strName);
    if (iterFind != m_mapLoadedImages.end())
    {
        return iterFind->second;
    }
    return nullptr;
}

Image* ImageLoader::getImageByPath(const std::string_view& strPath)
{
    for (const auto& pair : m_mapLoadedImages)
    {
        if (pair.second && pair.second->getPath() == strPath)
        {
            return pair.second;
        }
    }

    auto pImage = new Image(strPath);
    if (pImage->isCPULoaded())
    {
        pImage->loadTextureToGL();
        pImage->freeCPUData();
        m_mapLoadedImages[strPath] = pImage;
        return pImage;
    }
    return nullptr;
}

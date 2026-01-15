#include "image_loader.h"

#include "../core/window.h"

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

Image* ImageLoader::getImageByPath(const std::string_view& strPath)
{
    auto it = m_mapLoadedImages.find(strPath);
    if (it != m_mapLoadedImages.end())
    {
        return it->second;;
    }

    auto pImage = new Image(strPath);
    if (pImage->isCPULoaded())
    {
        switch (Window::ins->getGraphicAPI())
        {
            case GraphicAPI::OpenGL:
                pImage->loadTextureToGL();
                break;
#if __APPLE__
            case GraphicAPI::Metal:
                pImage->loadTextureToMetal();
                break;
#endif
            default:
                break;
        }

        pImage->freeCPUData();
        m_mapLoadedImages[strPath] = pImage;
        return pImage;
    }
    return nullptr;
}

#include "image_loader.h"

#include "core/renderer.h"

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
    m_pPureWhite1by1Image = new Image(1, 1, 4, new unsigned char[4]{255, 255, 255, 255});

    if (Renderer::isUsingOpenGL())
    {
        m_pPureWhite1by1Image->loadTextureToGL();
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        m_pPureWhite1by1Image->loadTextureToMetal();
    }
#endif
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
        switch (Renderer::getGraphicAPI())
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

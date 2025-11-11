#pragma once

#include "image.h"

class ImageLoader
{
public:
    inline static ImageLoader* getInstance() { return ins; }

    static void Initialize();
    static void Cleanup();

    void registerImage(const std::string_view& strName, const std::string_view& strPath);
    Image* getImage(const std::string_view& strName);
    Image* getImageByPath(const std::string_view& strPath);

private:
    ImageLoader();
    ~ImageLoader();

    static inline ImageLoader* ins = nullptr;

    std::unordered_map<std::string_view, Image*> m_mapLoadedImages;
};

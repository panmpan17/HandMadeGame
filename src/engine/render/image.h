#pragma once

#include <string>
#include <unordered_map>

#if __APPLE__
#include <Metal/Metal.hpp>
#endif // __APPLE__

typedef unsigned int GLuint;

class aiTexture;


class Image
{
public:
    Image(const std::string& strPath, bool flipVertically = true);
    Image(const std::string_view& strPath, bool flipVertically = true);
    Image(const aiTexture* pAiTexture, const char* strName, bool flipVertically = true);
    ~Image();

    inline int getWidth() const { return m_nWidth; }
    inline int getHeight() const { return m_nHeight; }
    inline unsigned char* getData() const { return m_pData; }
    inline GLuint getTextureID() const { return m_nTextureID; }

    inline bool isCPULoaded() const { return m_pData != nullptr; }
    inline bool isGPULoaded() const { return m_nTextureID != 0; }

    inline const std::string& getPath() const { return m_strPath; }

    void loadTextureToGL();
#if __APPLE__
    void loadTextureToMetal();
#endif

    void freeCPUData();

private:
    int m_nWidth;
    int m_nHeight;
    int m_nChannels;
    unsigned char* m_pData;

    GLuint m_nTextureID = 0; // Texture ID for OpenGL texture binding
    MTL::Texture* m_pMetalTexture = nullptr; // Texture for Metal

    std::string m_strPath;
};

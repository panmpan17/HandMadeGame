#pragma once

#include <string>

typedef unsigned int GLuint;

class Image
{
public:
    Image(const std::string& strPath);
    ~Image();

    inline int getWidth() const { return m_nWidth; }
    inline int getHeight() const { return m_nHeight; }
    inline unsigned char* getData() const { return m_pData; }
    inline GLuint getTextureID() const { return m_nTextureID; }

    inline bool isCPULoaded() const { return m_pData != nullptr; }
    inline bool isGPULoaded() const { return m_nTextureID != 0; }

    void loadTextureToGL();
    void freeCPUData();

private:
    int m_nWidth;
    int m_nHeight;
    int m_nChannels;
    unsigned char* m_pData;
    GLuint m_nTextureID = 0; // Texture ID for OpenGL texture binding
};

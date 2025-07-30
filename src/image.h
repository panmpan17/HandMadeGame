#pragma once

#include <string>


class Image
{
public:
    Image(const std::string& strPath);
    ~Image();

    inline int getWidth() const { return m_nWidth; }
    inline int getHeight() const { return m_nHeight; }
    inline unsigned char* getData() const { return m_pData; }
    inline bool isValid() const { return m_pData != nullptr; }

private:
    int m_nWidth;
    int m_nHeight;
    int m_nChannels;
    unsigned char* m_pData;
};

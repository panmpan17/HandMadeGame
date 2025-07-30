#pragma once

#include <string>

typedef unsigned int GLuint;


class Shader
{
public:
    Shader(const std::string& strVertexShaderPath, const std::string& strFragmentShaderPath);
    Shader();
    ~Shader();

    inline GLuint getProgram() const { return m_nProgram; }

protected:
    GLuint m_nProgram;
};


class TestShader : public Shader
{
public:
    TestShader();

    inline GLuint getMvpLocation() const { return m_nMvpLocation; }
    inline GLuint getVertexArray() const { return m_nVertexArray; }

private:
    GLuint m_nMvpLocation, m_nVertexArray;
};


class ImageShader : public Shader
{
public:
    ImageShader();

    inline GLuint getMvpLocation() const { return m_nMvpLocation; }
    inline GLuint getTextureLocation() const { return m_nTextureLocation; }
    inline GLuint getVertexArray() const { return m_nVertexArray; }
    inline GLuint getColorLocation() const { return m_nColorLocation; }

private:
    GLuint m_nMvpLocation, m_nColorLocation, m_nTextureLocation, m_nVertexArray;
};
#pragma once

#include <string>

typedef unsigned int GLuint;

class Shader
{
public:
    Shader(const std::string& strVertexShaderPath, const std::string& strFragmentShaderPath);
    Shader();
    ~Shader();

    GLuint getProgram() const { return m_nProgram; }

private:
    GLuint m_nProgram;
};
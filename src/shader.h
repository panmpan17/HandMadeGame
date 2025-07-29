#pragma once

#include <string>
#include <linmath.h>

typedef unsigned int GLuint;

typedef struct Vertex
{
    vec2 pos;
    vec3 col;
} Vertex;

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
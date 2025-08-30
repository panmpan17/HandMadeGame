#pragma once

#include <string>

typedef unsigned int GLuint;


class Shader
{
public:
    static void checkShaderCompilResult(const std::string_view& strVertexShaderPath, GLuint nShader);

    // Shader(const std::string_view& strShaderName, const std::string_view &strVertexShaderPath, const std::string_view &strFragmentShaderPath);
    Shader(int nId, const std::string& strShaderName, const std::string &strVertexShaderPath, const std::string &strFragmentShaderPath);
    Shader();
    ~Shader();

    inline const std::string& getName() const { return m_strName; }
    inline int getId() const { return m_nId; }

    inline GLuint getProgram() const { return m_nProgram; }

    GLuint getUniformLocation(const std::string& name) const;
    GLuint getAttributeLocation(const std::string& name) const;

protected:
    GLuint m_nProgram;

    int m_nId;
    std::string m_strName;
};

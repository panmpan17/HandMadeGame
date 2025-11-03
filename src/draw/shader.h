#pragma once

#include <string>

typedef unsigned int GLuint;


inline constexpr std::string_view SHADER_UNIFORM_TEXTURE_0 = "u_tex0";
inline constexpr std::string_view SHADER_UNIFORM_TEXTURE_1 = "u_tex1";
inline constexpr std::string_view SHADER_UNIFORM_TEXTURE_2 = "u_tex2";
inline constexpr std::string_view SHADER_UNIFORM_MVP = "u_MVP";


struct ShaderUniformHandle
{
    GLuint m_nLocation;
    std::string_view m_strName;
};


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

    const ShaderUniformHandle* getUniformHandle(const std::string_view& strName);

    GLuint getAttributeLocation(const std::string& name) const;

    void reload();

protected:
    GLuint m_nProgram;
    GLuint m_nVertexShader;
    GLuint m_nFragmentShader;

    int m_nId;
    std::string m_strName;

    std::string m_strVertexShaderPath;
    std::string m_strFragmentShaderPath;

    ShaderUniformHandle m_arrUniformHandles[16];
    int m_nUniformHandleCount = 0;

    GLuint getUniformLocation(const std::string& name) const;
};

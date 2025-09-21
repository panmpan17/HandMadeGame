#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>

#include "vertex.h"
#include "shader.h"
#include "../file_utils.h"
#include "../debug_macro.h"


void Shader::checkShaderCompilResult(const std::string_view& strShaderPath, GLuint nShader)
{
    GLint isCompiled = 0;
    glGetShaderiv(nShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(nShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(nShader, maxLength, &maxLength, &errorLog[0]);
        LOGERRLN_EX("Shader '{}' compilation failed: {}", strShaderPath, std::string(errorLog.data()));
    }
}


Shader::Shader(int nId, const std::string& strShaderName, const std::string &strVertexShaderPath, const std::string &strFragmentShaderPath)
{
    m_nId = nId;
    m_strName = strShaderName;

    m_strVertexShaderPath = strVertexShaderPath;
    m_strFragmentShaderPath = strFragmentShaderPath;

    {
        auto reader = FileReader(strVertexShaderPath);
        if (!reader.isOpen())
        {
            throw std::runtime_error("Failed to open vertex shader file: " + strVertexShaderPath);
        }
        std::string vertex_shader_text = reader.readAll();
        reader.close();

        m_nVertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char* vertex_shader_source = vertex_shader_text.c_str();
        glShaderSource(m_nVertexShader, 1, &vertex_shader_source, NULL);
        glCompileShader(m_nVertexShader);

#if IS_DEBUG_VERSION
        checkShaderCompilResult(strVertexShaderPath, m_nVertexShader);
#endif
    }

    {
        auto reader = FileReader(strFragmentShaderPath);
        if (!reader.isOpen())
        {
            throw std::runtime_error("Failed to open fragment shader file: " + strFragmentShaderPath);
        }
        std::string fragment_shader_text = reader.readAll();
        reader.close();

        m_nFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragment_shader_source = fragment_shader_text.c_str();
        glShaderSource(m_nFragmentShader, 1, &fragment_shader_source, NULL);
        glCompileShader(m_nFragmentShader);

#if IS_DEBUG_VERSION
        checkShaderCompilResult(strFragmentShaderPath, m_nFragmentShader);
#endif
    }

    m_nProgram = glCreateProgram();
    glAttachShader(m_nProgram, m_nVertexShader);
    glAttachShader(m_nProgram, m_nFragmentShader);
    glLinkProgram(m_nProgram);
}

Shader::~Shader()
{
    if (m_nProgram)
    {
        glDeleteShader(m_nProgram);
    }
}

GLuint Shader::getUniformLocation(const std::string& name) const
{
    return glGetUniformLocation(m_nProgram, name.c_str());
}

GLuint Shader::getAttributeLocation(const std::string& name) const
{
    return glGetAttribLocation(m_nProgram, name.c_str());
}

void Shader::reload()
{
}

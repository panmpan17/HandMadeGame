#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "file_utils.h"
// #include "debug_macro.h"

Shader::Shader(const std::string& strVertexShaderPath, const std::string& strFragmentShaderPath)
{
    GLuint vertex_shader;
    GLuint fragment_shader;

    {
        auto reader = FileReader(strVertexShaderPath);
        if (!reader.isOpen())
        {
            throw std::runtime_error("Failed to open vertex shader file: " + strVertexShaderPath);
        }
        std::string vertex_shader_text = reader.readAll();
        reader.close();

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        const char* vertex_shader_source = vertex_shader_text.c_str();
        glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
        glCompileShader(vertex_shader);
    }

    {
        auto reader = FileReader(strFragmentShaderPath);
        if (!reader.isOpen())
        {
            throw std::runtime_error("Failed to open fragment shader file: " + strFragmentShaderPath);
        }
        std::string fragment_shader_text = reader.readAll();
        reader.close();

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragment_shader_source = fragment_shader_text.c_str();
        glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
        glCompileShader(fragment_shader);
    }

    m_nProgram = glCreateProgram();
    glAttachShader(m_nProgram, vertex_shader);
    glAttachShader(m_nProgram, fragment_shader);
    glLinkProgram(m_nProgram);
}

Shader::~Shader()
{
    if (m_nProgram)
    {
        glDeleteShader(m_nProgram);
    }
}
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
        
#if IS_DEBUG_VERSION
        checkShaderCompilResult(strVertexShaderPath, vertex_shader);
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

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragment_shader_source = fragment_shader_text.c_str();
        glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
        glCompileShader(fragment_shader);

#if IS_DEBUG_VERSION
        checkShaderCompilResult(strFragmentShaderPath, fragment_shader);
#endif
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

GLuint Shader::getUniformLocation(const std::string& name) const
{
    return glGetUniformLocation(m_nProgram, name.c_str());
}

GLuint Shader::getAttributeLocation(const std::string& name) const
{
    return glGetAttribLocation(m_nProgram, name.c_str());
}


ImageShader::ImageShader()
    : Shader(1, "image", "assets/shaders/image.vert", "assets/shaders/image.frag")
{
    m_nMvpLocation = glGetUniformLocation(m_nProgram, "MVP");
    m_nColorLocation = glGetUniformLocation(m_nProgram, "imageColor");
    m_nVPosLocation = glGetAttribLocation(m_nProgram, "vPos");
    m_nVUVLocation = glGetAttribLocation(m_nProgram, "vUV");
    m_nTextureLocation = glGetUniformLocation(m_nProgram, "u_tex0");
    m_nSpriteSheetXCountLocation = glGetUniformLocation(m_nProgram, "spriteSheetXCount");
    m_nSpriteSheetYCountLocation = glGetUniformLocation(m_nProgram, "spriteSheetYCount");
    m_nUVOffsetLocation = glGetUniformLocation(m_nProgram, "uvOffset");

    glUseProgram(m_nProgram);
    glUniform1i(m_nTextureLocation, 0); // Texture unit 0
    glUseProgram(0);
}

ParticleShader::ParticleShader()
    : Shader(2, "particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag")
{
    m_nMvpLocation = glGetUniformLocation(m_nProgram, "MVP");
    m_nColorLocation = glGetUniformLocation(m_nProgram, "particleColor");
    m_nVPosLocation = glGetAttribLocation(m_nProgram, "vPos");
}

ParticleInstanceShader::ParticleInstanceShader()
    : Shader(3, "particle_instance", "assets/shaders/particle_instance.vert", "assets/shaders/particle_instance.frag")
{
    m_nMvpLocation = glGetUniformLocation(m_nProgram, "MVP");
    m_nNodeTransformLocation = glGetUniformLocation(m_nProgram, "NodeTransform");
    m_nUseNodeTransformLocation = glGetUniformLocation(m_nProgram, "UseNodeTransform");
    m_nUseTextureLocation = glGetUniformLocation(m_nProgram, "UseTexture");
    m_nTextureLocation = glGetUniformLocation(m_nProgram, "u_tex0");
    // m_nColorLocation = glGetUniformLocation(m_nProgram, "particleColor");
    // m_nVPosLocation = glGetAttribLocation(m_nProgram, "vPos");
}
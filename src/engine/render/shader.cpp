#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <sstream>

#include "vertex.h"
#include "shader.h"
#include "image.h"
#include "lighting/light_manager.h"
#include "../core/debug_macro.h"
#include "../core/camera.h"
#include "../core/time.h"
#include "../../utils/file_utils.h"


void checkShaderCompilResult(const std::string_view& strShaderPath, GLuint nShader)
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
        LOGERR("Shader '{}' compilation failed: {}", strShaderPath, std::string(errorLog.data()));
    }
}

void readShaderFile(const std::string& strFolderPath, std::ostringstream& ss)
{
    auto oReader = FileReader(strFolderPath);
    if (!oReader.isOpen())
    {
        throw std::runtime_error("Failed to open shader file: " + strFolderPath);
    }

    {
        std::string strLine;
        while (oReader.readLine(strLine))
        {
            if (memcmp(strLine.data(), "#include", 8) == 0)
            {
                size_t nFirstQuoteIndex = strLine.find("\"", 8);
                size_t nLastQuoteIndex = strLine.find("\"", nFirstQuoteIndex + 1);
                if (nFirstQuoteIndex == std::string::npos || nLastQuoteIndex == std::string::npos || nLastQuoteIndex <= nFirstQuoteIndex)
                {
                    throw std::runtime_error("Invalid #include directive in shader file: " + strFolderPath);
                }

                std::string strIncludePath = strLine.substr(nFirstQuoteIndex + 1, nLastQuoteIndex - nFirstQuoteIndex - 1);
                readShaderFile(strIncludePath, ss);
            }
            else
            {
                ss << strLine << '\n';
            }
        }
    }

    oReader.close();
}

GLuint loadShaderFileIntoGPU(std::string strFolderPath, bool isVertexShader)
{
    std::ostringstream ss;
    readShaderFile(strFolderPath, ss);

    GLuint nShader = glCreateShader(isVertexShader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);

    std::string strShaderSource = ss.str();
    const char* strShaderSourceConstChars = strShaderSource.c_str();
    glShaderSource(nShader, 1, &strShaderSourceConstChars, NULL);
    glCompileShader(nShader);

#if IS_DEBUG_VERSION
    checkShaderCompilResult(strFolderPath, nShader);
#endif

    return nShader;
}

Shader::Shader(int nId, const std::string& strShaderName, const std::string &strVertexShaderPath, const std::string &strFragmentShaderPath)
{
    m_nId = nId;
    m_strName = strShaderName;

    m_strVertexShaderPath = strVertexShaderPath;
    m_strFragmentShaderPath = strFragmentShaderPath;

    m_nVertexShader = loadShaderFileIntoGPU(strVertexShaderPath, true);
    m_nFragmentShader = loadShaderFileIntoGPU(strFragmentShaderPath, false);

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

bool Shader::getIsUsingFile(const std::string& strFilePath) const
{
    return (strFilePath == m_strVertexShaderPath || strFilePath == m_strFragmentShaderPath);
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
    glDeleteShader(m_nVertexShader);
    glDeleteShader(m_nFragmentShader);
    glDeleteProgram(m_nProgram);

    m_nVertexShader = loadShaderFileIntoGPU(m_strVertexShaderPath, true);
    m_nFragmentShader = loadShaderFileIntoGPU(m_strFragmentShaderPath, false);

    m_nProgram = glCreateProgram();
    glAttachShader(m_nProgram, m_nVertexShader);
    glAttachShader(m_nProgram, m_nFragmentShader);
    glLinkProgram(m_nProgram);

    for (int i = 0; i < m_nUniformHandleCount; ++i)
    {
        m_arrUniformHandles[i].m_nLocation = getUniformLocation(std::string(m_arrUniformHandles[i].m_strName));
    }

    reloadCameraUBOBinding();
    reloadLightUBOBinding();
    reloadTimeDataUBOBinding();
}

const ShaderUniformHandle* Shader::getUniformHandle(const std::string_view& strName)
{
    for (int i = 0; i < m_nUniformHandleCount; ++i)
    {
        if (m_arrUniformHandles[i].m_strName == strName)
        {
            return &m_arrUniformHandles[i];
        }
    }

    if (m_nUniformHandleCount >= 16)
    {
        LOGLN("Exceeded maximum number of uniform handles in shader '{}'", m_strName);
        return nullptr;
    }

    GLuint nLocation = getUniformLocation(std::string(strName));
    ShaderUniformHandle* pHandle = &m_arrUniformHandles[m_nUniformHandleCount++];
    pHandle->m_nLocation = nLocation;
    pHandle->m_strName = strName;
    return pHandle;
}

void Shader::setCameraUBOBindingPoint(GLuint nBindingPoint)
{
    m_nCameraUBOBindingPoint = nBindingPoint;
    reloadCameraUBOBinding();
}

void Shader::reloadCameraUBOBinding()
{
    if (m_nCameraUBOBindingPoint != GL_INVALID_INDEX && Camera::main)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, m_nCameraUBOBindingPoint, Camera::main->getCameraUBO());
        GLuint viewProjIndex = glGetUniformBlockIndex(m_nProgram, SHADER_GLOBAL_UNIFORM_CAMERA_MATRICES.data());
        glUniformBlockBinding(m_nProgram, viewProjIndex, m_nCameraUBOBindingPoint);
    }
}

void Shader::setLightUBOBindingPoint(GLuint nBindingPoint)
{
    m_nLightUBOBindingPoint = nBindingPoint;
    reloadLightUBOBinding();
}

void Shader::reloadLightUBOBinding()
{
    if (m_nLightUBOBindingPoint != GL_INVALID_INDEX && LightManager::getInstance())
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, m_nLightUBOBindingPoint, LightManager::getInstance()->getLightingUBO());
        GLuint lightIndex = glGetUniformBlockIndex(m_nProgram, SHADER_GLOBAL_UNIFORM_LIGHTING_DATA.data());
        glUniformBlockBinding(m_nProgram, lightIndex, m_nLightUBOBindingPoint);
    }
}

void Shader::setTimeDataUBOBindingPoint(GLuint nBindingPoint)
{
    m_nTimeDataUBOBindingPoint = nBindingPoint;
    reloadTimeDataUBOBinding();
}

void Shader::reloadTimeDataUBOBinding()
{
    if (m_nTimeDataUBOBindingPoint != GL_INVALID_INDEX && TimeManager::getInstance())
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, m_nTimeDataUBOBindingPoint, TimeManager::getInstance()->getTimeUBO());
        GLuint timeIndex = glGetUniformBlockIndex(m_nProgram, SHADER_GLOBAL_UNIFORM_TIME_DATA.data());
        glUniformBlockBinding(m_nProgram, timeIndex, m_nTimeDataUBOBindingPoint);
    }
}

bool ShaderUniformHandle::sendData(const ShaderUniformHandle* const pHandle, const mat4x4& matrix)
{
    if (!pHandle || pHandle->m_nLocation == GL_INVALID_INDEX)
    {
        return false;
    }

    glUniformMatrix4fv(pHandle->m_nLocation, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&matrix));
    return true;
}

bool ShaderUniformHandle::sendTexture(const ShaderUniformHandle* const pHandle, const Image* const pImage, int nIndex)
{
    if (!pHandle || pHandle->m_nLocation == GL_INVALID_INDEX || !pImage || !pImage->isGPULoaded())
    {
        return false;
    }

    
    glActiveTexture(GL_TEXTURE0 + nIndex);
    glBindTexture(GL_TEXTURE_2D, pImage->getTextureID());
    glUniform1i(pHandle->m_nLocation, nIndex);
    return true;
}

bool ShaderUniformHandle::sendTexture(const ShaderUniformHandle* const pHandle, GLuint nTextureId, int nIndex)
{
    if (!pHandle || pHandle->m_nLocation == GL_INVALID_INDEX)
    {
        return false;
    }

    
    glActiveTexture(GL_TEXTURE0 + nIndex);
    glBindTexture(GL_TEXTURE_2D, nTextureId);
    glUniform1i(pHandle->m_nLocation, nIndex);
    return true;
}

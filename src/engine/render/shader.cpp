#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <sstream>

#include "vertex.h"
#include "shader_loader.h"
#include "image.h"
// #include "lighting/light_manager.h"
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

Shader* Shader::loadFromOpenGLShader(const ShaderRegisteryData& pData)
{
    Shader* pShader = new Shader();
    pShader->m_nId = pData.nCurrentShaderId;
    pShader->m_strName = pData.m_strName;

    pShader->m_strVertexShaderPath = pData.m_strVertexPath;
    pShader->m_strFragmentShaderPath = pData.m_strFragmentPath;
    pShader->m_nVertexShader = loadShaderFileIntoGPU(pShader->m_strVertexShaderPath, true);
    pShader->m_nFragmentShader = loadShaderFileIntoGPU(pShader->m_strFragmentShaderPath, false);

    pShader->m_nProgram = glCreateProgram();
    glAttachShader(pShader->m_nProgram, pShader->m_nVertexShader);
    glAttachShader(pShader->m_nProgram, pShader->m_nFragmentShader);
    glLinkProgram(pShader->m_nProgram);

    if (pData.nCameraUBOIndex != GL_INVALID_INDEX)
    {
        pShader->setCameraUBOBindingPoint(pData.nCameraUBOIndex);
    }
    if (pData.nLightUBOIndex != GL_INVALID_INDEX)
    {
        pShader->setLightUBOBindingPoint(pData.nLightUBOIndex);
    }
    if (pData.nTimeDataUBOIndex != GL_INVALID_INDEX)
    {
        pShader->setTimeDataUBOBindingPoint(pData.nTimeDataUBOIndex);
    }

    return pShader;
}

#if __APPLE__
void metalShaderAddFloatAttribute(MTL::VertexDescriptor* const pVertexDesc, int nAttributeIndex, int nFloatCount)
{
    MTL::VertexAttributeDescriptor* const pAttrib0 = pVertexDesc->attributes()->object(nAttributeIndex);
    switch (nFloatCount)
    {
        case 1:
            pAttrib0->setFormat(MTL::VertexFormatFloat);
            break;
        case 2:
            pAttrib0->setFormat(MTL::VertexFormatFloat2);
            break;
        case 3:
            pAttrib0->setFormat(MTL::VertexFormatFloat3);
            break;
        case 4:
            pAttrib0->setFormat(MTL::VertexFormatFloat4);
            break;
        default:
            throw std::runtime_error("Unsupported float count for vertex attribute in Metal shader");
    }
    pAttrib0->setOffset(0);
    pAttrib0->setBufferIndex(nAttributeIndex);

    MTL::VertexBufferLayoutDescriptor* pLayout0 = pVertexDesc->layouts()->object(nAttributeIndex);
    pLayout0->setStride(sizeof(float) * nFloatCount); // Jump 8 bytes per vertex
    pLayout0->setStepRate(1);
    pLayout0->setStepFunction(MTL::VertexStepFunctionPerVertex);
}

Shader* Shader::loadFromMetalShader(MTL::Library* const pLibrary, MTL::Device* const pDevice, const ShaderRegisteryData& oData)
{
    std::string strFullVertexName = oData.m_strMetalShaderPrefix + "_vertexMain";
    std::string strFullFragmentName = oData.m_strMetalShaderPrefix + "_fragmentMain";

    MTL::Function* pVertexFunction = pLibrary->newFunction(NS::String::string(strFullVertexName.c_str(), NS::UTF8StringEncoding));
    MTL::Function* pFragmentFunction = pLibrary->newFunction(NS::String::string(strFullFragmentName.c_str(), NS::UTF8StringEncoding));
    if (!pVertexFunction || !pFragmentFunction)
    {
        LOGLN("Failed to load Metal shader functions. {}: {}, {}: {}",
            strFullVertexName, pVertexFunction ? "Loaded" : "Not Found",
            strFullFragmentName, pFragmentFunction ? "Loaded" : "Not Found");
        return nullptr;
    }

    MTL::RenderPipelineDescriptor* psoDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    psoDesc->setVertexFunction(pVertexFunction);
    psoDesc->setFragmentFunction(pFragmentFunction);

    MTL::RenderPipelineColorAttachmentDescriptor* pColorAttachment = psoDesc->colorAttachments()->object(0);
    pColorAttachment->setPixelFormat(MTL::PixelFormatBGRA8Unorm);

    if (oData.m_bTransparent)
    {
        pColorAttachment->setBlendingEnabled(true);

        pColorAttachment->setRgbBlendOperation(MTL::BlendOperationAdd);
        pColorAttachment->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
        pColorAttachment->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);

        pColorAttachment->setAlphaBlendOperation(MTL::BlendOperationAdd);
        pColorAttachment->setSourceAlphaBlendFactor(MTL::BlendFactorSourceAlpha);
        pColorAttachment->setDestinationAlphaBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
    }


    int nSize = static_cast<int>(oData.m_metalAttributeSizes.size());
    if (nSize > 0)
    {
        MTL::VertexDescriptor* pVertexDesc = MTL::VertexDescriptor::alloc()->init();
        for (int i = 0; i < nSize; ++i)
        {
            metalShaderAddFloatAttribute(pVertexDesc, i, oData.m_metalAttributeSizes[i]);
        }
        psoDesc->setVertexDescriptor(pVertexDesc);
        pVertexDesc->release();
    }

    Shader* pShader = new Shader;

    NS::Error* pError = nullptr;
    pShader->m_pPSO = pDevice->newRenderPipelineState(psoDesc, &pError);
    if (!pShader->m_pPSO)
    {
        // std::cerr << "Failed to create PSO: " << pError->localizedDescription()->utf8String() << std::endl;
        LOGLN("Failed to create PSO: {}", pError->localizedDescription()->utf8String());
        return nullptr;
    }

    pShader->m_strName = oData.m_strName;

    pVertexFunction->release();
    pFragmentFunction->release();
    psoDesc->release();

    return pShader;
}
#endif // __APPLE__

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
    pHandle->m_strName = std::string(strName);
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
    // if (m_nLightUBOBindingPoint != GL_INVALID_INDEX && LightManager::getInstance())
    // {
    //     glBindBufferBase(GL_UNIFORM_BUFFER, m_nLightUBOBindingPoint, LightManager::getInstance()->getLightingUBO());
    //     GLuint lightIndex = glGetUniformBlockIndex(m_nProgram, SHADER_GLOBAL_UNIFORM_LIGHTING_DATA.data());
    //     glUniformBlockBinding(m_nProgram, lightIndex, m_nLightUBOBindingPoint);
    // }
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

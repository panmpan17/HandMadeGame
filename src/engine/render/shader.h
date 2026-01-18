#pragma once

#include <string>
#include <linmath.h>

typedef unsigned int GLuint;
#define GL_INVALID_INDEX 0xFFFFFFFF

inline constexpr std::string_view SHADER_UNIFORM_TEXTURE_0 = "u_tex0";
inline constexpr std::string_view SHADER_UNIFORM_TEXTURE_1 = "u_tex1";
inline constexpr std::string_view SHADER_UNIFORM_TEXTURE_2 = "u_tex2";
inline constexpr std::string_view SHADER_UNIFORM_TEXTURE_3 = "u_tex3";
inline constexpr std::string_view SHADER_UNIFORM_TEXTURE_4 = "u_tex4";
inline constexpr std::string_view SHADER_UNIFORM_MVP = "u_MVP";

inline constexpr std::string_view SHADER_GLOBAL_UNIFORM_CAMERA_MATRICES = "CameraMatrices";
inline constexpr std::string_view SHADER_GLOBAL_UNIFORM_LIGHTING_DATA = "LightData";
inline constexpr std::string_view SHADER_GLOBAL_UNIFORM_TIME_DATA = "TimeData";

#if __APPLE__
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#endif


struct ShaderRegisteryData;
class Image;


struct ShaderUniformHandle
{
    GLuint m_nLocation;
    std::string m_strName;

    static bool sendData(const ShaderUniformHandle* const pHandle, const mat4x4& matrix);

    static bool sendTexture(const ShaderUniformHandle* const pHandle, const Image* const pImage, int nIndex);
    static bool sendTexture(const ShaderUniformHandle* const pHandle, GLuint nTextureId, int nIndex);
};


class Shader
{
public:
    static Shader* loadFromOpenGLShader(const ShaderRegisteryData& pData);

#if __APPLE__
    static Shader* loadFromMetalShader(MTL::Library* const pLibrary, MTL::Device* const pDevice, const ShaderRegisteryData& oData);
#endif // __APPLE__

    ~Shader();

    inline const std::string& getName() const { return m_strName; }
    inline int getId() const { return m_nId; }

    inline GLuint getProgram() const { return m_nProgram; }

    bool getIsUsingFile(const std::string& strFilePath) const;

    const ShaderUniformHandle* getUniformHandle(const std::string_view& strName);

    GLuint getAttributeLocation(const std::string& name) const;

    void reload();

    void setCameraUBOBindingPoint(GLuint nBindingPoint);
    void reloadCameraUBOBinding();

    void setLightUBOBindingPoint(GLuint nBindingPoint);
    void reloadLightUBOBinding();

    void setTimeDataUBOBindingPoint(GLuint nBindingPoint);
    void reloadTimeDataUBOBinding();

    inline bool isTransparent() const { return m_bTransparent; }

protected:
    Shader() {}

    GLuint m_nProgram = GL_INVALID_INDEX;
    GLuint m_nVertexShader = GL_INVALID_INDEX;
    GLuint m_nFragmentShader = GL_INVALID_INDEX;

    GLuint m_nCameraUBOBindingPoint = GL_INVALID_INDEX;
    GLuint m_nLightUBOBindingPoint = GL_INVALID_INDEX;
    GLuint m_nTimeDataUBOBindingPoint = GL_INVALID_INDEX;

    int m_nId;
    std::string m_strName;

    std::string m_strVertexShaderPath;
    std::string m_strFragmentShaderPath;

    ShaderUniformHandle m_arrUniformHandles[16];
    int m_nUniformHandleCount = 0;

    GLuint getUniformLocation(const std::string& name) const;

    bool m_bTransparent = false;

#if __APPLE__
public:
    inline MTL::RenderPipelineState* getMetalPipelineState() const { return m_pPSO; }

protected:
    MTL::RenderPipelineState* m_pPSO = nullptr;
#endif // __APPLE__
};

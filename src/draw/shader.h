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

    inline GLuint getProgram() const { return m_nProgram; }

    GLuint getUniformLocation(const std::string& name) const;
    GLuint getAttributeLocation(const std::string& name) const;

protected:
    GLuint m_nProgram;

    int m_nId;
    std::string m_strName;
};


class ImageShader : public Shader
{
public:
    ImageShader();

    inline GLuint getMvpLocation() const { return m_nMvpLocation; }
    inline GLuint getColorLocation() const { return m_nColorLocation; }
    inline GLuint getVPosLocation() const { return m_nVPosLocation; }
    inline GLuint getVUVLocation() const { return m_nVUVLocation; }
    inline GLuint getTextureLocation() const { return m_nTextureLocation; }

    inline GLuint getSpriteSheetXCountLocation() const { return m_nSpriteSheetXCountLocation; }
    inline GLuint getSpriteSheetYCountLocation() const { return m_nSpriteSheetYCountLocation; }
    inline GLuint getUVOffsetLocation() const { return m_nUVOffsetLocation; }

private:
    GLuint m_nMvpLocation, m_nColorLocation, m_nVPosLocation, m_nVUVLocation, m_nTextureLocation;
    GLuint m_nSpriteSheetXCountLocation, m_nSpriteSheetYCountLocation, m_nUVOffsetLocation;
};


class ParticleShader : public Shader
{
public:
    ParticleShader();

    inline GLuint getMvpLocation() const { return m_nMvpLocation; }
    inline GLuint getColorLocation() const { return m_nColorLocation; }
    inline GLuint getVPosLocation() const { return m_nVPosLocation; }
    // inline GLuint getVUVLocation() const { return m_nVUVLocation; }
    // inline GLuint getTextureLocation() const { return m_nTextureLocation; }

private:
    GLuint m_nMvpLocation, m_nColorLocation, m_nVPosLocation;
};



class ParticleInstanceShader : public Shader
{
public:
    ParticleInstanceShader();

    inline GLuint getMvpLocation() const { return m_nMvpLocation; }
    inline GLuint getNodeTransformLocation() const { return m_nNodeTransformLocation; }
    inline GLuint getUseNodeTransformLocation() const { return m_nUseNodeTransformLocation; }
    inline GLuint getUseTextureLocation() const { return m_nUseTextureLocation; }
    // inline GLuint getVUVLocation() const { return m_nVUVLocation; }
    inline GLuint getTextureLocation() const { return m_nTextureLocation; }

private:
    GLuint m_nMvpLocation, m_nNodeTransformLocation, m_nUseNodeTransformLocation, m_nUseTextureLocation, m_nTextureLocation;
};

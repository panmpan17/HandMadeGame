#pragma once


class Shader;
class Image;


class Material
{
public:
    Material(Shader* pShader) : m_pShader(pShader) {}
    ~Material() {}

    inline Shader* getShader() const { return m_pShader; }

    inline void setAlbedoMap(Image* pAlbedoMap) { m_pAlbedoMap = pAlbedoMap; }
    inline Image* getAlbedoMap() const { return m_pAlbedoMap; } 

    inline void setNormalMap(Image* pNormalMap) { m_pNormalMap = pNormalMap; }
    inline Image* getNormalMap() const { return m_pNormalMap; }

    inline void setSpecularMap(Image* pSpecularMap) { m_pSpecularMap = pSpecularMap; }
    inline Image* getSpecularMap() const { return m_pSpecularMap; }

private:
    Shader* m_pShader = nullptr;

    Image* m_pAlbedoMap = nullptr;
    Image* m_pNormalMap = nullptr;
    Image* m_pSpecularMap = nullptr;
};
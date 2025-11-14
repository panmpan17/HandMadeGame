#pragma once

#include <string_view>
#include <vector>
#include "../../utils/expandable_array.h"


class Shader;
class ShaderUniformHandle;
class Image;


struct TextureUniform
{
    const ShaderUniformHandle* pUniformHandle = nullptr;
    Image* pImage = nullptr;
};


class Material
{
public:
    Material(Shader* pShader) : m_pShader(pShader) {}
    ~Material() {}

    inline Shader* getShader() const { return m_pShader; }

    void bindTextureWithImage(const std::string_view& strUniformName, Image* pImage);

    void useShader() const;

    int sendTexturesData() const;

private:
    Shader* m_pShader = nullptr;

    std::vector<TextureUniform> m_vecTextureUniforms;

    void initShader(Shader* const pShader);
};
#pragma once

#include <string_view>


class Shader;
class ShaderUniformHandle;


class Skybox
{
public:
    Skybox();
    ~Skybox();

    void loadSkyboxCubmaps(const std::string_view& strRight, const std::string_view& strLeft,
                           const std::string_view& strTop, const std::string_view& strBottom,
                           const std::string_view& strFront, const std::string_view& strBack);

    void draw();

private:
    // Skybox textures
    unsigned int m_nSkyboxTextureID, m_nSkyboxVAO, m_nSkyboxVBO;
    Shader* m_pSkyboxShader = nullptr;

    void initShader(Shader* const pShader);
    void bindVertexArray();
};
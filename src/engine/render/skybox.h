#pragma once

#include <string_view>
#include <initializer_list>

#if __APPLE__
#include <Metal/Metal.hpp>
#endif // __APPLE__


class Shader;
class ShaderUniformHandle;


class Skybox
{
public:
    Skybox();
    ~Skybox();

    void loadSkyboxCubmaps(std::initializer_list<std::string_view> strImages);

    void draw();

private:
    // Skybox textures
    unsigned int m_nSkyboxTextureID, m_nSkyboxVAO, m_nSkyboxVBO;
    Shader* m_pSkyboxShader = nullptr;

    void initShader(Shader* const pShader);
    void bindVertexArray();

    void drawWithOpenGL();

#if __APPLE__
    MTL::Texture* m_pSkyboxMetalTexture = nullptr;
    MTL::Buffer* m_pMetalVertexBuffer = nullptr;

    void drawWithMetal();
#endif // __APPLE__
};
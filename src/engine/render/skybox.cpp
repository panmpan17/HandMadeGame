#include "skybox.h"

#include <glad/gl.h>
#include "image.h"
#include "shader.h"
#include "shader_loader.h"
#include "renderer.h"
#include "../core/camera.h"
#include "../core/window.h"


#define NS_INT(x) static_cast<NS::UInteger>(x)


Skybox::Skybox()
{
    m_pSkyboxShader = ShaderLoader::getInstance()->getShader("skybox");
    initShader(m_pSkyboxShader);
}

Skybox::~Skybox()
{
}

void Skybox::initShader(Shader* const pShader)
{
    bindVertexArray();
}

void Skybox::bindVertexArray()
{
    float arrSkyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    if (Window::ins->isUsingOpenGL())
    {
        glGenVertexArrays(1, &m_nSkyboxVAO);
        glGenBuffers(1, &m_nSkyboxVBO);

        glBindVertexArray(m_nSkyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_nSkyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(arrSkyboxVertices), &arrSkyboxVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        MTL::Device* pDevice = Window::ins->getMetalDevice();

        m_pMetalVertexBuffer = pDevice->newBuffer(
            arrSkyboxVertices,
            sizeof(arrSkyboxVertices),
            MTL::ResourceStorageModeShared);
    }
#endif // __APPLE__
}

void Skybox::loadSkyboxCubmaps(std::initializer_list<std::string_view> strImages)
{
    if (static_cast<int>(strImages.size()) != 6)
    {
        LOGERR("Skybox requires 6 images for cubemap");
        return;
    }

    bool bUsingOpenGL = Window::ins->isUsingOpenGL();
    if (bUsingOpenGL)
    {
        glGenTextures(1, &m_nSkyboxTextureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_nSkyboxTextureID);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

#if __APPLE__
    bool bUsingMetal = Window::ins->isUsingMetal();
    MTL::TextureDescriptor* pDesc;

    if (bUsingMetal)
    {
        pDesc = MTL::TextureDescriptor::alloc()->init();
        pDesc->setTextureType(MTL::TextureTypeCube);
        pDesc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
        pDesc->setWidth(2048);
        pDesc->setHeight(2048);
        pDesc->setArrayLength(1);

        m_pSkyboxMetalTexture = Window::ins->getMetalDevice()->newTexture(pDesc);
    }
#endif // __APPLE__

    for (int i = 0; i < 6; ++i)
    {
        Image oImage(*(strImages.begin() + i), false);
        if (oImage.isCPULoaded())
        {
            if (bUsingOpenGL)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, oImage.getWidth(), oImage.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, oImage.getData());
            }
#if __APPLE__
            else if (bUsingMetal)
            {
                int nWidth = oImage.getWidth();
                int nHeight = oImage.getHeight();

                MTL::Region region = MTL::Region::Make2D(0, 0, NS_INT(nWidth), NS_INT(nHeight));
                int nBytesPerRow = nWidth * 4;
                m_pSkyboxMetalTexture->replaceRegion(region, 0, i, oImage.getData(), NS_INT(nBytesPerRow), NS_INT(nBytesPerRow * nHeight));
            }
#endif // __APPLE__
        }
    }

    if (bUsingOpenGL)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
#if __APPLE__
    else if (bUsingMetal)
    {
        pDesc->release();
    }
#endif // __APPLE__
}

void Skybox::draw()
{
    bool bUsingOpenGL = Window::ins->isUsingOpenGL();
    bool bUsingMetal = Window::ins->isUsingMetal();
    
    if (bUsingOpenGL)
    {
        glDepthMask(GL_FALSE);

        drawWithOpenGL();

        glDepthMask(GL_TRUE);
    }
#if __APPLE__
    else if (bUsingMetal)
    {
        MTL::RenderCommandEncoder* pRenderEncoder = Window::ins->getCurrentFrameRenderEncoder();
        pRenderEncoder->setDepthStencilState(Renderer::m_pSkyboxStencilState);

        drawWithMetal();

        pRenderEncoder->setDepthStencilState(Renderer::m_pDepthOnStencilState);
    }
#endif // __APPLE__
}

void Skybox::drawWithOpenGL()
{
    glDepthFunc(GL_LEQUAL);

    glUseProgram(m_pSkyboxShader->getProgram());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_nSkyboxTextureID);

    glBindVertexArray(m_nSkyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    INCREASE_DRAW_CALL_COUNT(12);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glDepthFunc(GL_LESS);
}

#if __APPLE__
void Skybox::drawWithMetal()
{
    MTL::RenderCommandEncoder* pRenderCommandEncoder = Window::ins->getCurrentFrameRenderEncoder();

    pRenderCommandEncoder->setRenderPipelineState(m_pSkyboxShader->getMetalPipelineState());
    pRenderCommandEncoder->setVertexBuffer(m_pMetalVertexBuffer, 0, 0);
    pRenderCommandEncoder->setVertexBuffer(Camera::main->getCameraMetalUBO(), 0, 1);

    pRenderCommandEncoder->setFragmentTexture(m_pSkyboxMetalTexture, 0);
    pRenderCommandEncoder->setFragmentSamplerState(Renderer::m_pLinearSampler, 0);

    pRenderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS_INT(0), NS_INT(36));
    INCREASE_DRAW_CALL_COUNT(12);
}
#endif // __APPLE__

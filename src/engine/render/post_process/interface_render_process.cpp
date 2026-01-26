#include "interface_render_process.h"

#include <glad/gl.h>
#include "render_process_queue.h"
#include "../vertex.h"
#include "../shader_loader.h"
#include "../../core/debug_macro.h"
#include "../../core/window.h"


void IRenderProcess::registerShaderPosAndUV(Shader* pShader)
{
    GLuint nVPosAttr = pShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr = pShader->getAttributeLocation("a_vUV");
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nVUVAttr);
    glVertexAttribPointer(nVUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));
}

void IRenderProcess::initializeRenderTextureAndFBO(GLuint& nFBO, GLuint& nTexture, int nWidth, int nHeight, bool bGenerateFramebuffer/* = true*/)
{
    if (nTexture != 0 && nTexture != GL_INVALID_INDEX)
    {
        glDeleteTextures(1, &nTexture);
    }

    if (bGenerateFramebuffer)
    {
        glGenFramebuffers(1, &nFBO);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, nFBO);

    glGenTextures(1, &nTexture);
    glBindTexture(GL_TEXTURE_2D, nTexture);

    // Set the texture's format and size to match your window
    if (RenderProcessQueue::sm_bAllowHDR)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, nWidth, nHeight, 0, GL_RGB, GL_FLOAT, NULL);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }

    // Set texture parameters for correct filtering and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach the texture to the FBO's color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGERR("Framebuffer is not complete!");
    }
}

#if __APPLE__
void IRenderProcess::initializeRenderTextureAndFBO(MTL::Texture*& pTexture, int nWidth, int nHeight)
{
    if (pTexture)
    {
        pTexture->release();
        pTexture = nullptr;
    }

    MTL::TextureDescriptor* pTextureDesc = MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormatRGBA8Unorm, nWidth, nHeight, false);
    pTextureDesc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    pTexture = Window::ins->getMetalDevice()->newTexture(pTextureDesc);
}
#endif // __APPLE__

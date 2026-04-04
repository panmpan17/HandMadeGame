#pragma once

#if __APPLE__
#include <Metal/Metal.hpp>
#endif // __APPLE__

typedef unsigned int GLuint;

class RenderProcessQueue;
class Shader;

class IRenderProcess
{
public:
    IRenderProcess(RenderProcessQueue* pQueue) : m_pProcessQueue(pQueue) {}
    virtual ~IRenderProcess() = default;

    virtual void renderProcess() = 0;

    inline bool isActive() const { return m_bActive; }
    inline void setActive(bool bActive) { m_bActive = bActive; }

    virtual void initialize() = 0;

    virtual void onWindowResize() {}

protected:
    RenderProcessQueue* m_pProcessQueue = nullptr;
    bool m_bActive = true;

    static void registerShaderPosAndUV(Shader* pShader);
    static void initializeRenderTextureAndFBO(GLuint& nFBO, GLuint& nTexture, int nWidth, int nHeight, bool bGenerateFramebuffer = true);
#if __APPLE__
    static void initializeRenderTextureAndFBO(MTL::Texture*& pTexture, int nWidth, int nHeight);
#endif // __APPLE__
};
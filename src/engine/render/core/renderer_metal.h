#pragma once

#if __APPLE__
#include <Metal/Metal.hpp>
#include "../../core/window.h"


class MetalRenderer
{
public:
    static void initializeSamplers(MTL::Device* const pDevice)
    {
        { // Linear Sampler
            MTL::SamplerDescriptor* pSamplerDesc = MTL::SamplerDescriptor::alloc()->init();

            pSamplerDesc->setMinFilter(MTL::SamplerMinMagFilterLinear);
            pSamplerDesc->setMagFilter(MTL::SamplerMinMagFilterLinear);

            pSamplerDesc->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
            pSamplerDesc->setTAddressMode(MTL::SamplerAddressModeClampToEdge);

            pSamplerDesc->setCompareFunction(MTL::CompareFunctionNever);

            m_pLinearSampler = pDevice->newSamplerState(pSamplerDesc);

            pSamplerDesc->release();
        }

        MTL::DepthStencilDescriptor* pOpaqueDesc = MTL::DepthStencilDescriptor::alloc()->init();
        pOpaqueDesc->setDepthCompareFunction(MTL::CompareFunctionLess);
        pOpaqueDesc->setDepthWriteEnabled(true);
        m_pDepthOnStencilState = pDevice->newDepthStencilState(pOpaqueDesc);
        pOpaqueDesc->release();

        MTL::DepthStencilDescriptor* pTransDesc = MTL::DepthStencilDescriptor::alloc()->init();
        pTransDesc->setDepthCompareFunction(MTL::CompareFunctionLess);
        pTransDesc->setDepthWriteEnabled(false);
        m_pDepthOffStencilState = pDevice->newDepthStencilState(pTransDesc);
        pTransDesc->release();

        MTL::DepthStencilDescriptor* pSkyboxDesc = MTL::DepthStencilDescriptor::alloc()->init();
        pSkyboxDesc->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
        pSkyboxDesc->setDepthWriteEnabled(false);
        m_pSkyboxStencilState = pDevice->newDepthStencilState(pSkyboxDesc);
        pSkyboxDesc->release();
    }

    static void initMetalDepthTexture(MTL::Device* const pDevice, const int width, const int height)
    {
        MTL::TextureDescriptor* pTextureDesc = MTL::TextureDescriptor::alloc()->init();
        pTextureDesc->setPixelFormat(MTL::PixelFormatDepth32Float);
        pTextureDesc->setWidth(width);
        pTextureDesc->setHeight(height);
        pTextureDesc->setUsage(MTL::TextureUsageRenderTarget);
        pTextureDesc->setStorageMode(MTL::StorageModePrivate);

        if (m_pDepthTexture)
        {
            m_pDepthTexture->release();
        }
        m_pDepthTexture = pDevice->newTexture(pTextureDesc);

        pTextureDesc->release();
    }

    static void initRenderPassDescriptor()
    {
        if (!sm_pRenderPassDescriptor)
        {
            sm_pRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
        }

        MTL::RenderPassDepthAttachmentDescriptor* pDepthAttach = sm_pRenderPassDescriptor->depthAttachment();
        pDepthAttach->setTexture(MetalRenderer::m_pDepthTexture);
        pDepthAttach->setLoadAction(MTL::LoadActionClear);
        pDepthAttach->setClearDepth(1.0);
        pDepthAttach->setStoreAction(MTL::StoreActionDontCare);

        MTL::RenderPassColorAttachmentDescriptor* pColorAttachment = sm_pRenderPassDescriptor->colorAttachments()->object(0);
        pColorAttachment->setLoadAction(MTL::LoadActionClear);
        pColorAttachment->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));
        pColorAttachment->setStoreAction(MTL::StoreActionStore);
    }

    static inline MTL::RenderPassDescriptor* getRenderPassDescriptor() { return sm_pRenderPassDescriptor; }

    static void cleanup()
    {
        if (sm_pRenderPassDescriptor)
        {
            sm_pRenderPassDescriptor->release();
            sm_pRenderPassDescriptor = nullptr;
        }
        if (sm_pDepthOnlyRenderPassDescriptor)
        {
            sm_pDepthOnlyRenderPassDescriptor->release();
            sm_pDepthOnlyRenderPassDescriptor = nullptr;
        }
    }


    static inline MTL::SamplerState* m_pLinearSampler = nullptr;
    static inline MTL::DepthStencilState* m_pDepthOnStencilState = nullptr;
    static inline MTL::DepthStencilState* m_pDepthOffStencilState = nullptr;
    static inline MTL::DepthStencilState* m_pSkyboxStencilState = nullptr;
    static inline MTL::Texture* m_pDepthTexture = nullptr;

private:
    static inline MTL::RenderPassDescriptor* sm_pRenderPassDescriptor = nullptr;


#pragma region Lighting shadow depth rendering

public:
    static inline MTL::RenderPassDescriptor* getDepthOnlyRenderPassDescriptor() { return sm_pDepthOnlyRenderPassDescriptor; }
    static inline MTL::RenderCommandEncoder* getCurrentFrameDepthRenderEncoder() { return sm_pCurrentFrameDepthRenderEncoder; }

    static void initDepthOnlyRenderPassDescriptor()
    {
        if (!sm_pDepthOnlyRenderPassDescriptor)
        {
            sm_pDepthOnlyRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
        }

        MTL::RenderPassDepthAttachmentDescriptor* pDepthOnlyAttach = sm_pDepthOnlyRenderPassDescriptor->depthAttachment();
        pDepthOnlyAttach->setTexture(nullptr); // Lighting system will set the depth texture when render depth
        pDepthOnlyAttach->setLoadAction(MTL::LoadActionClear);
        pDepthOnlyAttach->setClearDepth(1.0);
        pDepthOnlyAttach->setStoreAction(MTL::StoreActionStore);
    }

    static void startNewDepthOnlyFrame(MTL::Texture* const pTexture)
    {
        MTL::RenderPassDescriptor* pDepthOnlyRenderPassDescriptor = MetalRenderer::getDepthOnlyRenderPassDescriptor();
        sm_pDepthOnlyRenderPassDescriptor->depthAttachment()->setTexture(pTexture);

        sm_pCurrentFrameDepthRenderEncoder = Window::ins->getCurrentCommandBuffer()->renderCommandEncoder(pDepthOnlyRenderPassDescriptor);
        sm_pCurrentFrameDepthRenderEncoder->setDepthStencilState(MetalRenderer::m_pDepthOnStencilState);
        sm_pCurrentFrameDepthRenderEncoder->setCullMode(MTL::CullModeFront); // Use front-face culling for shadow maps to reduce shadow acne
        sm_pCurrentFrameDepthRenderEncoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
    }

    static void endDepthOnlyFrame()
    {
        sm_pCurrentFrameDepthRenderEncoder->endEncoding();
        sm_pCurrentFrameDepthRenderEncoder = nullptr;
    }

private:
    static inline MTL::RenderPassDescriptor* sm_pDepthOnlyRenderPassDescriptor = nullptr;
    static inline MTL::RenderCommandEncoder* sm_pCurrentFrameDepthRenderEncoder = nullptr;

#pragma endregion
};

#endif // __APPLE__
#if __APPLE__
#include <Metal/Metal.hpp>
#endif // __APPLE__


class MetalRenderer
{
#if __APPLE__
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

    static inline MTL::SamplerState* m_pLinearSampler = nullptr;
    static inline MTL::DepthStencilState* m_pDepthOnStencilState = nullptr;
    static inline MTL::DepthStencilState* m_pDepthOffStencilState = nullptr;
    static inline MTL::DepthStencilState* m_pSkyboxStencilState = nullptr;
    static inline MTL::Texture* m_pDepthTexture = nullptr;
#endif
};
#pragma once


#if __APPLE__
#include <Metal/Metal.hpp>
#endif // __APPLE__


class Renderer
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
        pTransDesc->setDepthWriteEnabled(false); // <--- CRITICAL
        m_pDepthOffStencilState = pDevice->newDepthStencilState(pTransDesc);
        pTransDesc->release();
    }

    static inline MTL::SamplerState* m_pLinearSampler = nullptr;
    static inline MTL::DepthStencilState* m_pDepthOnStencilState = nullptr;
    static inline MTL::DepthStencilState* m_pDepthOffStencilState = nullptr;
#endif
};
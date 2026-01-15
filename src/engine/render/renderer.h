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
    }

    static inline MTL::SamplerState* m_pLinearSampler = nullptr;
#endif
};
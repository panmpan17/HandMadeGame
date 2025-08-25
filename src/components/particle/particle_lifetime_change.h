#pragma once

#include "particle_system.h"


class ScaleThroughParticleLifetime : public IParticleIndividualModule
{
public:
    ScaleThroughParticleLifetime(float fMin, float fMax) : m_fMinScale(fMin), m_fMaxScale(fMax) {}

    void update(ParticleSystem& particleSystem, ParticleGPUInstance* pParticleGpu, ParticleCPUInstance* pParticleCpu, float deltaTime) override
    {
        float fLifeTimeRatio = pParticleCpu->m_fLifetime / pParticleCpu->m_fMaxLifetime;
        float fScaleMultiplier = m_fMinScale + (m_fMaxScale - m_fMinScale) * (1.0f - fLifeTimeRatio);
        pParticleGpu->m_fScale = pParticleCpu->m_fBaseScale * fScaleMultiplier;
    }

private:
    float m_fMinScale, m_fMaxScale;
};

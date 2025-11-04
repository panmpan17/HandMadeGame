#pragma once

#include "particle_system.h"


class ScaleThroughParticleLifetime : public IParticleIndividualModule
{
public:
    ScaleThroughParticleLifetime() {}
    ScaleThroughParticleLifetime(float fMin, float fMax) : m_fMinScale(fMin), m_fMaxScale(fMax) {}

    void update(ParticleSystem& particleSystem, ParticleGPUInstance* pParticleGpu, ParticleCPUInstance* pParticleCpu, float deltaTime) override
    {
        float fLifeTimeRatio = pParticleCpu->m_fLifetime / pParticleCpu->m_fMaxLifetime;
        float fScaleMultiplier = m_fMinScale + (m_fMaxScale - m_fMinScale) * (1.0f - fLifeTimeRatio);
        pParticleGpu->m_fScale = pParticleCpu->m_fBaseScale * fScaleMultiplier;
    }

    std::string getDeserializedValue() const override;
    void deserializeFromField(const std::string_view& strFieldValue) override;

private:
    float m_fMinScale = 0, m_fMaxScale = 0;
};

REGISTER_CLASS(ScaleThroughParticleLifetime)


class OpacityThroughParticleLifetime : public IParticleIndividualModule
{
public:
    OpacityThroughParticleLifetime() {}
    OpacityThroughParticleLifetime(float fMin, float fMax) : m_fMinOpacity(fMin), m_fMaxOpacity(fMax) {}

    void update(ParticleSystem& particleSystem, ParticleGPUInstance* pParticleGpu, ParticleCPUInstance* pParticleCpu, float deltaTime) override
    {
        float fLifeTimeRatio = pParticleCpu->m_fLifetime / pParticleCpu->m_fMaxLifetime;
        float fOpacity = m_fMinOpacity + (m_fMaxOpacity - m_fMinOpacity) * (1.0f - fLifeTimeRatio);
        pParticleGpu->m_fOpacity = fOpacity;
    }

    std::string getDeserializedValue() const override;
    void deserializeFromField(const std::string_view& strFieldValue) override;

private:
    float m_fMinOpacity = 0, m_fMaxOpacity = 0;
};

REGISTER_CLASS(OpacityThroughParticleLifetime)

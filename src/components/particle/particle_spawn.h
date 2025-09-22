#pragma once

#include "particle_system.h"


class ParticleIntervalSpawn : public IParticleModule
{
public:
    ParticleIntervalSpawn() {}
    ParticleIntervalSpawn(float fInterval) : m_fSpawnInterval(fInterval) {}
    ParticleIntervalSpawn(int nParticlePerSeconds) : m_fSpawnInterval(1.0f / nParticlePerSeconds) {}

    void update(ParticleSystem& rParticleSystem, float fDeltaTime) override
    {
        if (!rParticleSystem.getIsEmitting())
        {
            return;
        }

        m_fSpawnTimer += fDeltaTime;
        if (m_fSpawnTimer >= m_fSpawnInterval)
        {
            rParticleSystem.spawnNewParticles(1);
            m_fSpawnTimer = 0.0f;
        }
    }

    std::string getDeserializedValue() const override;
    void deserializeFromField(const std::string_view& strFieldValue) override;

private:
    float m_fSpawnInterval;
    float m_fSpawnTimer = 0.0f;
};

REGISTER_CLASS(ParticleIntervalSpawn)

class ParticleBurstSpawn : public IParticleModule
{
public:
    ParticleBurstSpawn() {}
    ParticleBurstSpawn(float fBurstAt, int nBurstCount) : m_fBurstAt(fBurstAt), m_nBurstCount(nBurstCount) {}

    void update(ParticleSystem& rParticleSystem, float fDeltaTime) override
    {
        if (!rParticleSystem.getIsEmitting() || m_bHasBursted)
        {
            return;
        }

        if (rParticleSystem.getActiveTimer() >= m_fBurstAt)
        {
            rParticleSystem.spawnNewParticles(m_nBurstCount);
            m_bHasBursted = true;
        }
    }

    std::string getDeserializedValue() const override;
    void deserializeFromField(const std::string_view& strFieldValue) override;

    void onActiveTimeReset() override
    {
        m_bHasBursted = false;
    }

private:
    float m_fBurstAt = 0.1f; // Activated time
    bool m_bHasBursted = false;
    int m_nBurstCount;
};

REGISTER_CLASS(ParticleBurstSpawn)

#pragma once

#include "particle_system.h"


class ParticleIntervalSpawn : public IParticleModule
{
public:
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

private:
    float m_fSpawnInterval;
    float m_fSpawnTimer = 0.0f;
};

class ParticleBurstSpawn : public IParticleModule
{
public:
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

    void onActiveTimeReset() override
    {
        m_bHasBursted = false;
    }

private:
    float m_fBurstAt = 0.1f; // Activated time
    bool m_bHasBursted = false;
    int m_nBurstCount;
};

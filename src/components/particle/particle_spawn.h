#pragma once

#include "particle_system.h"


class ParticleIntervalSpawn : public IParticleModule
{
public:
    ParticleIntervalSpawn(float fInterval) : m_fSpawnInterval(fInterval) {}
    ParticleIntervalSpawn(int nParticlePerSeconds) : m_fSpawnInterval(1.0f / nParticlePerSeconds) {}

    void update(ParticleSystem& rParticleSystem, float fDeltaTime) override
    {
        m_fSpawnTimer += fDeltaTime;
        if (m_fSpawnTimer >= m_fSpawnInterval)
        {
            rParticleSystem.spawnNewParticle();
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
    ParticleBurstSpawn(float fBurstInterval, int nBurstCount) : m_fBurstInterval(fBurstInterval), m_nBurstCount(nBurstCount) {}

    void update(ParticleSystem& rParticleSystem, float fDeltaTime) override
    {
        m_fBurstTimer += fDeltaTime;

        if (m_fBurstTimer >= m_fBurstInterval)
        {
            int nStartIndex = 0;
            for (int i = 0; i < m_nBurstCount; ++i)
            {
                nStartIndex = rParticleSystem.spawnNewParticle(nStartIndex) + 1;
            }
            m_fBurstTimer = 0.0f;
        }
    }

private:
    float m_fBurstInterval = 0.1f; // Time between bursts
    float m_fBurstTimer = 0.0f;
    int m_nBurstCount;
};

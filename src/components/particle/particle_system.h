#pragma once

// #include "../component.h"
#include "../../draw/drawable_interface.h"
#include <linmath.h>

typedef unsigned int GLuint;

struct ParticleGPUInstance
{
    vec2 m_vecPosition;
    vec4 m_vecColor;
    float m_fRotation;
    float m_fScale;
};

struct ParticleCPUInstance
{
    float m_fRotationSpeed;
    // vec2 velocity;
    float m_fLifetime;

    bool isAlive() const { return m_fLifetime > 0; }
};


class ParticleSystem;
class IParticleModule
{
public:
    virtual void update(ParticleSystem& particleSystem, float deltaTime) = 0;
};


class ParticleSystem : public IDrawable
{
public:
    ParticleSystem(int nParticleCount);

    ~ParticleSystem() override;

    bool isUpdatable() const override { return true; }

    
    void registerBuffer() override;
    void draw() override;
    void setShader(Shader* pShader) override { m_pShader = pShader; }

    void update(float fDeltaTime) override;

    /// @brief Spawns a new particle
    /// @param nStartIndex The index to start searching for a free particle
    /// @return The index of the newly spawned particle, or -1 if none available
    int spawnNewParticle(int nStartIndex = 0);

    void addParticleModule(IParticleModule* pModule) { for (int i = 0; i < 4; ++i) { if (m_arrParticleModules[i] == nullptr) { m_arrParticleModules[i] = pModule; break; } } }

private:
    ParticleGPUInstance* m_arrParticlesGPU = nullptr;
    ParticleCPUInstance* m_arrParticlesCPU = nullptr;

    int m_nAllParticleCount = 0;
    int m_nAliveParticleCount = 0;
    int m_nLastAliveParticleIndex = 0;

    GLuint m_nVertexBuffer = 0;
    GLuint m_nVertexArray = 0;
    GLuint m_nInstanceBuffer = 0;

    Shader* m_pShader = nullptr;

    bool m_bSimulateInLocal = false;

    IParticleModule* m_arrParticleModules[4];

    void sortAliveParticleInFront();
};

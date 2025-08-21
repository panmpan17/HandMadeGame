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
    vec2 m_vecVelocity;
    float m_fLifetime;

    bool isAlive() const { return m_fLifetime > 0; }
};


class ParticleSystem;
class IParticleModule
{
public:
    virtual ~IParticleModule() = default;

    virtual void update(ParticleSystem& particleSystem, float deltaTime) = 0;
};


enum class eParticleSpawnShape : int
{
    DOT,
    CIRCLE,
    BOX,
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

    /// @brief Spawns new particles
    /// @param nSpawnCount The number of particles to spawn
    void spawnNewParticles(int nSpawnCount = 0);

    void addParticleModule(IParticleModule* pModule) { for (int i = 0; i < 4; ++i) { if (m_arrParticleModules[i] == nullptr) { m_arrParticleModules[i] = pModule; break; } } }

    void setParticleLifetime(float fMin, float fMax) { m_fLifetimeMin = fMin; m_fLifetimeMax = fMax; }
    void setParticleStartRotation(float fMin, float fMax) { m_fStartRotationMin = fMin; m_fStartRotationMax = fMax; }
    void setParticleStartRotationSpeed(float fMin, float fMax) { m_fStartRotationSpeedMin = fMin; m_fStartRotationSpeedMax = fMax; }
    void setParticleStartScale(float fMin, float fMax) { m_fStartScaleMin = fMin; m_fStartScaleMax = fMax; }
    void setParticleStartColor(const vec4& colorMin, const vec4& colorMax) { vec4_dup(m_vecStartColorMin, colorMin); vec4_dup(m_vecStartColorMax, colorMax); }

    void setSpawnShape(eParticleSpawnShape shape) { m_eSpawnShape = shape; }
    void setSpawnShapeDimensions(float width, float height) { m_fSpawnShapeWidth = width; m_fSpawnShapeHeight = height; }

private:
    ParticleGPUInstance* m_arrParticlesGPU = nullptr;
    ParticleCPUInstance* m_arrParticlesCPU = nullptr;

    GLuint m_nVertexBuffer = 0;
    GLuint m_nVertexArray = 0;
    GLuint m_nInstanceBuffer = 0;

    Shader* m_pShader = nullptr;

    int m_nAllParticleCount = 0;
    int m_nAliveParticleCount = 0;
    int m_nLastAliveParticleIndex = 0;

    eParticleSpawnShape m_eSpawnShape = eParticleSpawnShape::DOT;
    float m_fSpawnShapeWidth = 0.1f;
    float m_fSpawnShapeHeight = 0.1f;

    float m_fLifetimeMin = 1.0f;
    float m_fLifetimeMax = 3.0f;

    float m_fStartRotationMin = 0.0f;
    float m_fStartRotationMax = 0.0f;
    float m_fStartRotationSpeedMin = 0.0f;
    float m_fStartRotationSpeedMax = 0.0f;

    float m_fStartScaleMin = 0.2f;
    float m_fStartScaleMax = 0.4f;

    vec4 m_vecStartColorMin = { 0.9f, 0.9f, 0.9f, 1.f };
    vec4 m_vecStartColorMax = { 1.f, 1.f, 1.f, 1.f };

    float m_fStartVelocityMin = 0.3f;
    float m_fStartVelocityMax = 0.5f;

    bool m_bSimulateInLocal = false;

    IParticleModule* m_arrParticleModules[4];

    void updateParticle(int& nIndex, float fDeltaTime);
    void sortAliveParticleInFront();
};

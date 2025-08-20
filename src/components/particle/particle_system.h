#pragma once

// #include "../component.h"
#include "../../draw/drawable_interface.h"
#include <linmath.h>

typedef unsigned int GLuint;

struct ParticleGPUInstance
{
    vec2 position;
    vec4 color;
    float rotation;
    float scale;
};

struct ParticleCPUInstance
{
    float rotationSpeed;
    // vec2 velocity;
    float lifetime;

    bool isAlive() const { return lifetime > 0; }
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

    void update(float deltaTime) override;

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

    void sortAliveParticleInFront();
};

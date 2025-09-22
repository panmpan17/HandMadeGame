#pragma once

// #include "../component.h"
#include "../drawable_interface.h"
#include <linmath.h>

typedef unsigned int GLuint;

struct Particle
{
    vec2 position;
    // vec2 velocity;
    vec4 color;
    float rotation;
    float rotationSpeed;
    float scale;
    // float life;
};

class SimpleParticleSystem : public IDrawable
{
public:
    SimpleParticleSystem(int nParticleCount);

    ~SimpleParticleSystem() override
    {
        delete[] m_arrParticles;
    }

    bool isUpdatable() const override { return true; }

    
    void registerBuffer() override;
    void draw() override;
    void setShader(Shader* pShader) override;

    void update(float deltaTime) override;

private:
    Particle* m_arrParticles = nullptr;

    int m_nParticleCount = 0;

    GLuint m_nVertexBuffer = 0;
    GLuint m_nVertexArray = 0;
    GLuint m_nMVPUniform, m_nColorUniform;

    Shader* m_pShader = nullptr;

    bool m_bSimulateInLocal = false;
};

#pragma once

#include "component.h"
#include "../draw/drawable_interface.h"
#include <linmath.h>
#include <random>

typedef unsigned int GLuint;

struct Particle
{
    vec2 position;
    // vec2 velocity;
    vec4 color;
    // float life;
};

class SimpleParticleSystem : public IDrawable
{
public:
    SimpleParticleSystem(int nParticleCount)
    {
        m_nParticleCount = nParticleCount;
        m_arrParticles = new Particle[nParticleCount];

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> positionDist(-0.5f, 0.5f);
        std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

        for (int i = 0; i < nParticleCount; ++i)
        {
            m_arrParticles[i].position[0] = positionDist(gen); // Initialize position
            m_arrParticles[i].position[1] = positionDist(gen);

            m_arrParticles[i].color[0] = colorDist(gen); // Initialize color to white
            m_arrParticles[i].color[1] = colorDist(gen);
            m_arrParticles[i].color[2] = colorDist(gen);
            m_arrParticles[i].color[3] = 1.0f; // Alpha
        }
    }

    ~SimpleParticleSystem() override
    {
        delete[] m_arrParticles;
    }

    bool isUpdatable() const override { return true; }

    
    void registerBuffer() override;
    void draw() override;
    void setShader(Shader* pShader) override { m_pShader = pShader; }

    void update(float deltaTime) override;

private:
    Particle* m_arrParticles = nullptr;

    int m_nParticleCount = 0;

    GLuint m_nVertexBuffer = 0;
    GLuint m_nVertexArray = 0;

    Shader* m_pShader = nullptr;
};

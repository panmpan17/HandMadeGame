#include "particle_system.h"

#include <glad/gl.h>
#include "../../draw/shader.h"
#include "../../draw/vertex.h"
#include "../../debug_macro.h"
#include "../../node.h"
#include "../../camera.h"
#include "../../window.h"


float randomFloat();
float randomFloat(float fMin, float fMax);
int randomInt(int nMin, int nMax);

#define SWAP_PARTICLE_POSITION(i, j) \
    do { \
        std::swap(m_arrParticlesGPU[i], m_arrParticlesGPU[j]); \
        std::swap(m_arrParticlesCPU[i], m_arrParticlesCPU[j]); \
    } while (0)


ParticleSystem::ParticleSystem(int nParticleCount)
{
    m_nAllParticleCount = nParticleCount;
    m_arrParticlesGPU = new ParticleGPUInstance[nParticleCount];
    m_arrParticlesCPU = new ParticleCPUInstance[nParticleCount];

    m_nAliveParticleCount = 0;
    m_nLastAliveParticleIndex = -1;

    for (int i = 0; i < nParticleCount; ++i)
    {
        m_arrParticlesCPU[i].m_fLifetime = 0;
    }

    for (int i = 0; i < 4; ++i)
    {
        m_arrParticleModules[i] = nullptr;
    }
}

ParticleSystem::~ParticleSystem()
{
    delete[] m_arrParticlesGPU;
    delete[] m_arrParticlesCPU;

    glDeleteBuffers(1, &m_nInstanceBuffer);
    glDeleteVertexArrays(1, &m_nVertexArray);
    glDeleteBuffers(1, &m_nVertexBuffer);
}

void ParticleSystem::registerBuffer()
{
    // Four corner vertex datas
    Vertex arrQuadVerticies[4];
    arrQuadVerticies[0] = { { -0.1f, -0.1f } };
    arrQuadVerticies[1] = { { 0.1f, -0.1f } };
    arrQuadVerticies[2] = { { -0.1f, 0.1f } };
    arrQuadVerticies[3] = { { 0.1f, 0.1f } };

    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrQuadVerticies), arrQuadVerticies, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));


    // Instance data
    glGenBuffers(1, &m_nInstanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nInstanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_nAllParticleCount * sizeof(ParticleGPUInstance), m_arrParticlesGPU, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, m_vecPosition));
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, m_vecColor));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, m_fRotation));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, m_fScale));
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    if (m_nAliveParticleCount <= 0) return;

    glBindVertexArray(m_nVertexArray);
    glUseProgram(m_pShader->getProgram());

    mat4x4 cameraViewMatrix;
    Camera::main->getViewMatrix(cameraViewMatrix);

    ParticleShader* pParticleShader = static_cast<ParticleShader*>(m_pShader);
    glUniformMatrix4fv(pParticleShader->getMvpLocation(), 1, GL_FALSE, (const GLfloat*) cameraViewMatrix);

    glBindBuffer(GL_ARRAY_BUFFER, m_nInstanceBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_nAliveParticleCount * sizeof(ParticleGPUInstance), m_arrParticlesGPU);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_nAliveParticleCount);
    INCREASE_DRAW_CALL_COUNT();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

void ParticleSystem::update(float fDeltaTime)
{
    // spawnNewParticle();
    for (IParticleModule*& pModule : m_arrParticleModules)
    {
        if (pModule)
        {
            pModule->update(*this, fDeltaTime);
        }
    }

    for (int i = 0; i < m_nAllParticleCount; ++i)
    {
        if (m_arrParticlesCPU[i].isAlive())
        {
            m_arrParticlesCPU[i].m_fLifetime -= fDeltaTime;

            if (!m_arrParticlesCPU[i].isAlive())
            {
                SWAP_PARTICLE_POSITION(i, m_nLastAliveParticleIndex);
                --m_nLastAliveParticleIndex;
                --m_nAliveParticleCount;
                --i;
                continue;
            }
            
            m_arrParticlesGPU[i].m_fRotation += fDeltaTime * m_arrParticlesCPU[i].m_fRotationSpeed; // Example m_fRotation update
        }
    }
}

int ParticleSystem::spawnNewParticle(int nStartIndex/* = 0*/)
{
    for (int i = nStartIndex; i < m_nAllParticleCount; ++i)
    {
        if (!m_arrParticlesCPU[i].isAlive())
        {
            m_arrParticlesCPU[i].m_fLifetime = randomFloat(1.0f, 3.0f); // Random lifetime
            m_arrParticlesCPU[i].m_fRotationSpeed = randomFloat(0.1f, 1.0f); // Random m_fRotation speed

            m_arrParticlesGPU[i].m_vecPosition[0] = randomFloat(-0.8f, 0.8f);
            m_arrParticlesGPU[i].m_vecPosition[1] = randomFloat(-0.8f, 0.8f);
            vec4_dup(m_arrParticlesGPU[i].m_vecColor, vec4 { 1.f, 1.f, 1.f, 1.f });
            m_arrParticlesGPU[i].m_fRotation = randomFloat(0.0f, 2.0f * M_PI);
            m_arrParticlesGPU[i].m_fScale = randomFloat(0.2f, 0.4f);

            ++m_nAliveParticleCount;
            return ++m_nLastAliveParticleIndex;
        }
    }

    return -1;
}

void ParticleSystem::sortAliveParticleInFront()
{
    for (int i = 0; i < m_nAllParticleCount; i++)
    {
        for (int e = i + 1; e < m_nAllParticleCount; e++)
        {
            if (!m_arrParticlesCPU[i].isAlive() && m_arrParticlesCPU[e].isAlive())
            {
                SWAP_PARTICLE_POSITION(i, e);
            }
        }
    }

    for (int i = 0; i < m_nAllParticleCount; i++)
    {
        if (m_arrParticlesCPU[i].isAlive())
        {
            m_nLastAliveParticleIndex = i;
        }
        else
        {
            break;
        }
    }
}
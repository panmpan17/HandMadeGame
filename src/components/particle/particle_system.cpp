#include "particle_system.h"

#include <glad/gl.h>
#include "../../draw/shader.h"
#include "../../draw/vertex.h"
#include "../../debug_macro.h"
#include "../../node.h"
#include "../../camera.h"
#include "../../window.h"
#include "../../random.h"


#define SWAP_PARTICLE_POSITION(i, j) \
    do { \
        if (i == j) break; \
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

    for (IParticleModule*& pModule : m_arrParticleModules)
    {
        if (pModule)
        {
            delete pModule;
        }
    }

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

void ParticleSystem::spawnNewParticles(int nSpawnCount/* = 1*/)
{
    for (int i = m_nLastAliveParticleIndex + 1, spawned = 0; i < m_nAllParticleCount && spawned < nSpawnCount; ++i, ++spawned)
    {
        if (!m_arrParticlesCPU[i].isAlive())
        {
            m_arrParticlesCPU[i].m_fLifetime = randomFloat(m_fLifetimeMin, m_fLifetimeMax); // Random lifetime
            m_arrParticlesCPU[i].m_fRotationSpeed = randomFloat(m_fStartRotationSpeedMin, m_fStartRotationSpeedMax); // Random m_fRotation speed

            const vec3& nodePosition = getNode()->getPosition();
            switch (m_eSpawnShape)
            {
                case eParticleSpawnShape::DOT:
                    vec2_dup(m_arrParticlesGPU[i].m_vecPosition, nodePosition);
                    break;

                case eParticleSpawnShape::CIRCLE:
                    {
                        float fAngle = randomFloat(0.0f, 2.0f * M_PI);
                        float fRadius = randomFloat(0.0f, m_fSpawnShapeWidth);
                        m_arrParticlesGPU[i].m_vecPosition[0] = cos(fAngle) * fRadius + nodePosition[0];
                        m_arrParticlesGPU[i].m_vecPosition[1] = sin(fAngle) * fRadius + nodePosition[1];
                    }
                    break;

                case eParticleSpawnShape::BOX:
                    m_arrParticlesGPU[i].m_vecPosition[0] = randomFloat(-m_fSpawnShapeWidth, m_fSpawnShapeWidth) + nodePosition[0];
                    m_arrParticlesGPU[i].m_vecPosition[1] = randomFloat(-m_fSpawnShapeHeight, m_fSpawnShapeHeight) + nodePosition[1];
                    break;
            }

            randomBetweenVec4(m_arrParticlesGPU[i].m_vecColor, m_vecStartColorMin, m_vecStartColorMax);
            m_arrParticlesGPU[i].m_fRotation = randomFloat(m_fStartRotationMin, m_fStartRotationMax);
            m_arrParticlesGPU[i].m_fScale = randomFloat(m_fStartScaleMin, m_fStartScaleMax);

            ++m_nAliveParticleCount;
            ++m_nLastAliveParticleIndex;
        }
    }
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
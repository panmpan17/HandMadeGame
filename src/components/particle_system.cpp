#include "particle_system.h"
#include "simple_particle_system.h"

#include <glad/gl.h>
#include "../draw/shader.h"
#include "../draw/vertex.h"
#include "../debug_macro.h"
#include "../node.h"
#include "../camera.h"
#include "../window.h"


float randomFloat();
float randomFloat(float fMin, float fMax);
int randomInt(int nMin, int nMax);


ParticleSystem::ParticleSystem(int nParticleCount)
{
    m_nParticleCount = nParticleCount;
    m_arrParticlesGPU = new ParticleGPUInstance[nParticleCount];
    m_arrParticlesCPU = new ParticleCPUInstance[nParticleCount];

    for (int i = 0; i < nParticleCount; ++i)
    {
        m_arrParticlesGPU[i].position[0] = randomFloat(-0.8f, 0.8f); // Initialize position
        m_arrParticlesGPU[i].position[1] = randomFloat(-0.8f, 0.8f);

        m_arrParticlesGPU[i].color[0] = randomFloat(); // Initialize color to white
        m_arrParticlesGPU[i].color[1] = randomFloat();
        m_arrParticlesGPU[i].color[2] = randomFloat();
        m_arrParticlesGPU[i].color[3] = 1.0f; // Alpha

        m_arrParticlesGPU[i].rotation = randomFloat(0.0f, 2.0f * M_PI); // Random rotation
        m_arrParticlesGPU[i].scale = randomFloat(0.2f, 0.4f); // Random scale

        m_arrParticlesCPU[i].rotationSpeed = randomFloat(0.1f, 1.0f); // Random rotation speed
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
    glBufferData(GL_ARRAY_BUFFER, m_nParticleCount * sizeof(ParticleGPUInstance), m_arrParticlesGPU, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, position));
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, color));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, rotation));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, scale));
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    glBindVertexArray(m_nVertexArray);
    glUseProgram(m_pShader->getProgram());

    mat4x4 cameraViewMatrix;
    Camera::main->getViewMatrix(cameraViewMatrix);

    ParticleShader* pParticleShader = static_cast<ParticleShader*>(m_pShader);
    glUniformMatrix4fv(pParticleShader->getMvpLocation(), 1, GL_FALSE, (const GLfloat*) cameraViewMatrix);

    glBindBuffer(GL_ARRAY_BUFFER, m_nInstanceBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_nParticleCount * sizeof(ParticleGPUInstance), m_arrParticlesGPU);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_nParticleCount);
    INCREASE_DRAW_CALL_COUNT();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

void ParticleSystem::update(float deltaTime)
{
    for (int i = 0; i < m_nParticleCount; ++i)
    {
        // Update particle position, rotation, etc.
        m_arrParticlesGPU[i].rotation += deltaTime * m_arrParticlesCPU[i].rotationSpeed; // Example rotation update
    }
}
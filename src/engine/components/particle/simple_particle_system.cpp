#include "simple_particle_system.h"

#include <glad/gl.h>
#include <random>
#include "../../render/shader.h"
#include "../../render/vertex.h"
#include "../../core/debug_macro.h"
#include "../../core/camera.h"
#include "../../core/window.h"
#include "../../core/scene/node.h"
#include "../../core/math/random.h"
#include "../../../utils/platform.h"


void printMatx4x4(mat4x4 mat)
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            std::cout << mat[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

SimpleParticleSystem::SimpleParticleSystem(int nParticleCount)
{
    m_nParticleCount = nParticleCount;
    m_arrParticles = new Particle[nParticleCount];

    for (int i = 0; i < nParticleCount; ++i)
    {
        m_arrParticles[i].position[0] = randomFloat(-0.8f, 0.8f); // Initialize position
        m_arrParticles[i].position[1] = randomFloat(-0.8f, 0.8f);

        m_arrParticles[i].color[0] = randomFloat(); // Initialize color to white
        m_arrParticles[i].color[1] = randomFloat();
        m_arrParticles[i].color[2] = randomFloat();
        m_arrParticles[i].color[3] = 1.0f; // Alpha

        m_arrParticles[i].rotation = randomFloat(0.0f, 2.0f * M_PI); // Random rotation
        m_arrParticles[i].scale = randomFloat(0.2f, 0.4f); // Random scale
        m_arrParticles[i].rotationSpeed = randomFloat(0.1f, 1.0f); // Random rotation speed
    }
}

void SimpleParticleSystem::setShader(Shader* pShader)
{
    m_pShader = pShader;

    m_pMVPUniForm = m_pShader->getUniformHandle(SHADER_UNIFORM_MVP);
    m_pColorUniform = m_pShader->getUniformHandle("u_particleColor");
}

void SimpleParticleSystem::registerBuffer()
{
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

    GLuint nVPosAttr = m_pShader->getAttributeLocation("a_vPos");

    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SimpleParticleSystem::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    glBindVertexArray(m_nVertexArray);
    glUseProgram(m_pShader->getProgram());

    for (int i = 0; i < m_nParticleCount; ++i)
    {
        mat4x4 mvp, local;

        mat4x4_identity(local);

        // Transform the local matrix
        mat4x4_scale_aniso(local, local, m_arrParticles[i].scale, m_arrParticles[i].scale, m_arrParticles[i].scale);

        const vec2& particlePosition = m_arrParticles[i].position;
        mat4x4 location;
        mat4x4_translate(location, particlePosition[0], particlePosition[1], 0.0f);
        mat4x4_mul(local, local, location);

        mat4x4_rotate_Z(local, local, m_arrParticles[i].rotation);
        
        // Apply to Camera matrix
        const mat4x4& cameraViewMatrix = Camera::main->getViewProjectionMatrix();
        mat4x4_mul(mvp, cameraViewMatrix, local);

        glUniformMatrix4fv(m_pMVPUniForm->m_nLocation, 1, GL_FALSE, (const GLfloat*) mvp);

        const vec4& color = m_arrParticles[i].color; // Use the color of the first particle for now
        glUniform4f(m_pColorUniform->m_nLocation, color[0], color[1], color[2], color[3]);

        // glUniform1i(m_pShader->getTextureLocation(), 0); // Texture unit 0

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, m_pImage ? m_pImage->getTextureID() : 0);
        // LOGLN("Quad::draw() - Using texture location: {}", m_pShader->getTextureLocation());

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
        INCREASE_DRAW_CALL_COUNT();
    }

    // glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

void SimpleParticleSystem::update(float deltaTime)
{
    for (int i = 0; i < m_nParticleCount; ++i)
    {
        // Update particle position, rotation, etc.
        m_arrParticles[i].rotation += deltaTime * m_arrParticles[i].rotationSpeed; // Example rotation update
    }
}
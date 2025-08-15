#include "simple_particle_system.h"

#include <glad/gl.h>
#include "../draw/shader.h"
#include "../draw/vertex.h"
#include "../debug_macro.h"
#include "../node.h"
#include "../camera.h"


void SimpleParticleSystem::registerBuffer()
{
    Vertex arrQuadVerticies[4];
    arrQuadVerticies[0] = { { -0.2f, -0.2f } };
    arrQuadVerticies[1] = { { 0.2f, -0.2f } };
    arrQuadVerticies[2] = { { -0.2f, 0.2f } };
    arrQuadVerticies[3] = { { 0.2f, 0.2f } };

    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrQuadVerticies), arrQuadVerticies, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);

    ParticleShader* pParticleShader = static_cast<ParticleShader*>(m_pShader);
    glEnableVertexAttribArray(pParticleShader->getVPosLocation());
    glVertexAttribPointer(pParticleShader->getVPosLocation(), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SimpleParticleSystem::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    // std::println("SimpleParticleSystem::draw() - Drawing particles");

    glBindVertexArray(m_nVertexArray);

    for (int i = 0; i < m_nParticleCount; ++i)
    {
        mat4x4 mvp, local, cameraViewMatrix;

        mat4x4_identity(local);

        const vec3& position = m_pNode->getPosition();
        mat4x4_translate(local, position[0] + m_arrParticles[i].position[0], position[1] + m_arrParticles[i].position[1], position[2]);

        // mat4x4_rotate_Z(local, local, m_pNode->getRotation());

        Camera::main->getViewMatrix(cameraViewMatrix);
        mat4x4_mul(mvp, cameraViewMatrix, local);

        glUseProgram(m_pShader->getProgram());

        ParticleShader* pParticleShader = static_cast<ParticleShader*>(m_pShader);
        glUniformMatrix4fv(pParticleShader->getMvpLocation(), 1, GL_FALSE, (const GLfloat*) mvp);

        const vec4& color = m_arrParticles[i].color; // Use the color of the first particle for now
        glUniform4f(pParticleShader->getColorLocation(), color[0], color[1], color[2], color[3]);

        // glUniform1i(m_pShader->getTextureLocation(), 0); // Texture unit 0

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, m_pImage ? m_pImage->getTextureID() : 0);
        // LOGLN_EX("Quad::draw() - Using texture location: {}", m_pShader->getTextureLocation());

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    }

    // glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

void SimpleParticleSystem::update(float deltaTime)
{

}
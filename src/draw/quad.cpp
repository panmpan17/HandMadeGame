// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include "quad.h"
#include "../camera.h"

Quad::Quad(float fX, float fY, float fWidth, float fHeight, vec3 color)
{
    float fStartX = fX - fWidth / 2.0f;
    float fStartY = fY - fHeight / 2.0f;
    m_arrVertices[0] = { { fStartX, fStartY }, *color }; // Bottom left
    m_arrVertices[1] = { { fStartX + fWidth, fStartY }, *color }; // Bottom right
    m_arrVertices[2] = { { fStartX, fStartY + fHeight }, *color }; // Top right
    m_arrVertices[3] = { { fStartX + fWidth, fStartY + fHeight }, *color }; // Top left
}

Quad::~Quad()
{
    glDeleteBuffers(1, &m_nVertexBuffer);
    glDeleteVertexArrays(1, &m_nVertexArray);
}

void Quad::registerBuffer()
{
    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_arrVertices), m_arrVertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    glEnableVertexAttribArray(m_pShader->getVPosLocation());
    glVertexAttribPointer(m_pShader->getVPosLocation(), 2, GL_FLOAT, GL_FALSE, sizeof(VertexWColor), (void*)offsetof(VertexWColor, pos));
    glEnableVertexAttribArray(m_pShader->getVColLocation());
    glVertexAttribPointer(m_pShader->getVColLocation(), 3, GL_FLOAT, GL_FALSE, sizeof(VertexWColor), (void*)offsetof(VertexWColor, col));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Quad::draw()
{
    mat4x4 mvp, local, cameraViewMatrix;

    mat4x4_identity(local);
    mat4x4_translate(local, m_position[0], m_position[1], m_position[2]);

    Camera::main->getViewMatrix(cameraViewMatrix);
    mat4x4_mul(mvp, cameraViewMatrix, local);

    glUseProgram(m_pShader->getProgram());
    glUniformMatrix4fv(m_pShader->getMvpLocation(), 1, GL_FALSE, (const GLfloat*) mvp);
    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    glUseProgram(0);
}
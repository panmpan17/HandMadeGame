// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include "quad.h"

Quad::Quad(float fX, float fY, float fWidth, float fHeight, vec3 color)
{
    float fStartX = fX - fWidth / 2.0f;
    float fStartY = fY - fHeight / 2.0f;
    m_arrVertices[0] = { { fStartX, fStartY }, *color }; // Bottom left
    m_arrVertices[1] = { { fStartX + fWidth, fStartY }, *color }; // Bottom right
    m_arrVertices[2] = { { fStartX, fStartY + fHeight }, *color }; // Top right
    m_arrVertices[3] = { { fStartX + fWidth, fStartY + fHeight }, *color }; // Top left

    registerBuffer();
}

Quad::~Quad()
{
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteVertexArrays(1, &m_vertexArray);
}

void Quad::registerBuffer()
{
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_arrVertices), m_arrVertices, GL_STATIC_DRAW);
}

void Quad::draw()
{
    mat4x4 mvp;
    mat4x4_identity(mvp);

    glUseProgram(m_pShader->getProgram());
    glUniformMatrix4fv(m_pShader->getMvpLocation(), 1, GL_FALSE, (const GLfloat*) mvp);
    glBindVertexArray(m_pShader->getVertexArray());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
}
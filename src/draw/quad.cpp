// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "quad.h"
#include "../image.h"
#include "../camera.h"
#include "../debug_macro.h"

Quad::Quad(float fX, float fY, float fWidth, float fHeight, vec4 color)
{
    float fStartX = fX - fWidth / 2.0f;
    float fStartY = fY - fHeight / 2.0f;
    m_arrVertices[0] = { { fStartX, fStartY }, { 0.0f, 0.0f } }; // Bottom left
    m_arrVertices[1] = { { fStartX + fWidth, fStartY }, { 1.0f, 0.0f } }; // Bottom right
    m_arrVertices[2] = { { fStartX, fStartY + fHeight }, { 0.0f, 1.0f } }; // Top right
    m_arrVertices[3] = { { fStartX + fWidth, fStartY + fHeight }, { 1.0f, 1.0f } }; // Top left

    vec4_dup(m_color, color);
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
    glVertexAttribPointer(m_pShader->getVPosLocation(), 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(m_pShader->getVUVLocation());
    glVertexAttribPointer(m_pShader->getVUVLocation(), 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

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
    glUniform4f(m_pShader->getColorLocation(), m_color[0], m_color[1], m_color[2], 1);

    glUniform1i(m_pShader->getTextureLocation(), 0); // Texture unit 0

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pImage ? m_pImage->getTextureID() : 0);
    LOGLN_EX("Quad::draw() - Using texture location: {}", m_pShader->getTextureLocation());

    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}
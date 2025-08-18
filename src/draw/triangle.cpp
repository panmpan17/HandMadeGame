#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "triangle.h"
#include "shader.h"
#include "../camera.h"
#include "../node.h"
#include "../window.h"

#include "../debug_macro.h"


Triangle::Triangle()
{
    vertices[0] = { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } };
    vertices[1] = { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } };
    vertices[2] = { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } };
}

Triangle::~Triangle()
{
}

void Triangle::registerBuffer()
{
    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

void Triangle::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the triangle");

    mat4x4 mvp, local, cameraViewMatrix;
    mat4x4_identity(local);

    const vec3& position = m_pNode->getPosition();
    mat4x4_translate(local, position[0], position[1], position[2]);

    mat4x4_rotate_Z(local, local, m_pNode->getRotation());

    Camera::main->getViewMatrix(cameraViewMatrix);
    mat4x4_mul(mvp, cameraViewMatrix, local);

    glUseProgram(m_pShader->getProgram());
    glUniformMatrix4fv(m_pShader->getMvpLocation(), 1, GL_FALSE, (const GLfloat*) mvp);
    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    INCREASE_DRAW_CALL_COUNT();
    glUseProgram(0);
}
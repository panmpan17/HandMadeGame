#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "triangle.h"
#include "shader.h"
#include "../camera.h"

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
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void Triangle::draw()
{
    mat4x4 mvp;
    Camera::main->getViewMatrix(mvp);

    ASSERT(m_pShader, "Shader must be set before drawing the triangle");
    
    glUseProgram(m_pShader->getProgram());
    glUniformMatrix4fv(m_pShader->getMvpLocation(), 1, GL_FALSE, (const GLfloat*) mvp);
    glBindVertexArray(m_pShader->getVertexArray());
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
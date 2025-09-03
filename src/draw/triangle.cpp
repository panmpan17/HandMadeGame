#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "triangle.h"
#include "shader.h"
#include "shader_loader.h"
#include "../camera.h"
#include "../node.h"
#include "../window.h"
#include "../serializer.h"

#include "../debug_macro.h"


Triangle::Triangle()
{
}

Triangle::~Triangle()
{
}

void Triangle::registerBuffer()
{
    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

    VertexWColor arrVertices[3];
    arrVertices[0] = { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } };
    arrVertices[1] = { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } };
    arrVertices[2] = { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } };
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

    GLuint nVPosAttr =  m_pShader->getAttributeLocation("a_vPos");
    GLuint nVColAttr =  m_pShader->getAttributeLocation("a_vCol");

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWColor), (void*)offsetof(VertexWColor, pos));
    glEnableVertexAttribArray(nVColAttr);
    glVertexAttribPointer(nVColAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWColor), (void*)offsetof(VertexWColor, col));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Triangle::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the triangle");

    mat4x4 mvp, local;
    mat4x4_identity(local);

    const vec3& position = m_pNode->getPosition();
    mat4x4_translate(local, position[0], position[1], position[2]);

    mat4x4_rotate_Z(local, local, m_pNode->getRotation());

    const mat4x4& cameraViewMatrix = Camera::main->getViewProjectionMatrix();
    mat4x4_mul(mvp, cameraViewMatrix, local);

    glUseProgram(m_pShader->getProgram());
    glUniformMatrix4fv(m_nMVPUniform, 1, GL_FALSE, (const GLfloat*) mvp);
    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    INCREASE_DRAW_CALL_COUNT();
    glUseProgram(0);
}

void Triangle::serializeToWrapper(DataSerializer& serializer) const
{
    if (m_pShader)
    {
        serializer.ADD_ATTRIBUTES_VALUE(m_pShader, m_pShader->getId());
    }
}

bool Triangle::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    IF_DESERIALIZE_FIELD_CHECK(m_pShader)
    {
        m_pShader = ShaderLoader::getInstance()->getShader(std::atoi(strFieldValue.data()));
        return true;
    }

    return false;
}

void Triangle::onNodeFinishedDeserialization()
{
    if (m_pShader)
    {
        setShader(m_pShader);
        registerBuffer();
    }
}
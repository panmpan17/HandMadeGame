#include "mesh_renderer.h"

#include <glad/gl.h>
#include "../models/simple_obj_reader.h"
#include "../draw/shader.h"
#include "../debug_macro.h"
#include "../window.h"
#include "../node.h"
#include "../camera.h"

MeshRenderer::MeshRenderer(SimpleObjReader* pMesh)
    : m_pMesh(pMesh)
{
}

MeshRenderer::~MeshRenderer()
{
    if (m_pMesh)
    {
        delete m_pMesh;
        m_pMesh = nullptr;
    }
}

void MeshRenderer::setShader(Shader* pShader)
{
    m_pShader = pShader;

    // m_nMVPUniform = m_pShader->getUniformLocation("u_MVP");
    m_nModelUniform = m_pShader->getUniformLocation("u_Model");
    m_nViewUniform = m_pShader->getUniformLocation("u_View");
    m_nProjectionUniform = m_pShader->getUniformLocation("u_Projection");

    bindVertexArray();
}

void MeshRenderer::bindTestingVertexArray()
{
    // For testing, create a simple triangle mesh
    VertexWUVNormal arrVertices[24] = {
        VertexWUVNormal { { -0.5f, -0.5f, 0.5f }, { 0.375f, 0.f }, { -1.f, -0.f, -0.f} },
        VertexWUVNormal { { -0.5f, 0.5f, 0.5f }, { 0.625f, 0.f }, { -1.f, -0.f, -0.f} },
        VertexWUVNormal { { -0.5f, 0.5f, -0.5f }, { 0.625f, 0.25f }, { -1.f, -0.f, -0.f} },
        VertexWUVNormal { { -0.5f, -0.5f, -0.5f }, { 0.375f, 0.25f }, { -1.f, -0.f, -0.f} },
        VertexWUVNormal { { -0.5f, -0.5f, -0.5f }, { 0.375f, 0.25f }, { -0.f, -0.f, -1.f} },
        VertexWUVNormal { { -0.5f, 0.5f, -0.5f }, { 0.625f, 0.25f }, { -0.f, -0.f, -1.f} },
        VertexWUVNormal { { 0.5f, 0.5f, -0.5f }, { 0.625f, 0.5f }, { -0.f, -0.f, -1.f} },
        VertexWUVNormal { { 0.5f, -0.5f, -0.5f }, { 0.375f, 0.5f }, { -0.f, -0.f, -1.f} },
        VertexWUVNormal { { 0.5f, -0.5f, -0.5f }, { 0.375f, 0.5f }, { 1.f, -0.f, -0.f} },
        VertexWUVNormal { { 0.5f, 0.5f, -0.5f }, { 0.625f, 0.5f }, { 1.f, -0.f, -0.f} },
        VertexWUVNormal { { 0.5f, 0.5f, 0.5f }, { 0.625f, 0.75f }, { 1.f, -0.f, -0.f} },
        VertexWUVNormal { { 0.5f, -0.5f, 0.5f }, { 0.375f, 0.75f }, { 1.f, -0.f, -0.f} },
        VertexWUVNormal { { 0.5f, -0.5f, 0.5f }, { 0.375f, 0.75f }, { -0.f, -0.f, 1.f} },
        VertexWUVNormal { { 0.5f, 0.5f, 0.5f }, { 0.625f, 0.75f }, { -0.f, -0.f, 1.f} },
        VertexWUVNormal { { -0.5f, 0.5f, 0.5f }, { 0.625f, 1.f }, { -0.f, -0.f, 1.f} },
        VertexWUVNormal { { -0.5f, -0.5f, 0.5f }, { 0.375f, 1.f }, { -0.f, -0.f, 1.f} },
        VertexWUVNormal { { -0.5f, -0.5f, -0.5f }, { 0.125f, 0.5f }, { -0.f, -1.f, -0.f} },
        VertexWUVNormal { { 0.5f, -0.5f, -0.5f }, { 0.375f, 0.5f }, { -0.f, -1.f, -0.f} },
        VertexWUVNormal { { 0.5f, -0.5f, 0.5f }, { 0.375f, 0.75f }, { -0.f, -1.f, -0.f} },
        VertexWUVNormal { { -0.5f, -0.5f, 0.5f }, { 0.125f, 0.75f }, { -0.f, -1.f, -0.f} },
        VertexWUVNormal { { 0.5f, 0.5f, -0.5f }, { 0.625f, 0.5f }, { -0.f, 1.f, -0.f} },
        VertexWUVNormal { { -0.5f, 0.5f, -0.5f }, { 0.875f, 0.5f }, { -0.f, 1.f, -0.f} },
        VertexWUVNormal { { -0.5f, 0.5f, 0.5f }, { 0.875f, 0.75f }, { -0.f, 1.f, -0.f} },
        VertexWUVNormal { { 0.5f, 0.5f, 0.5f }, { 0.625f, 0.75f }, { -0.f, 1.f, -0.f} },
    };

    TriangleFace arrFaces[12] = {
        TriangleFace { 0, 1, 2 },
        TriangleFace { 0, 2, 3 },
        TriangleFace { 4, 5, 6 },
        TriangleFace { 4, 6, 7 },
        TriangleFace { 8, 9, 10 },
        TriangleFace { 8, 10, 11 },
        TriangleFace { 12, 13, 14 },
        TriangleFace { 12, 14, 15 },
        TriangleFace { 16, 17, 18 },
        TriangleFace { 16, 18, 19 },
        TriangleFace { 20, 21, 22 },
        TriangleFace { 20, 22, 23 },
    };

    m_nVertexCount = 12 * 3;

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);

    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_nIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrFaces), arrFaces, GL_STATIC_DRAW);

    // Get the attribute locations from the shader
    GLuint nVPosAttr = m_pShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr = m_pShader->getAttributeLocation("a_vUV");
    GLuint nVNormalAttr = m_pShader->getAttributeLocation("a_vNormal");

    // Enable and set the vertex attributes using the retrieved locations
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, pos));

    glEnableVertexAttribArray(nVUVAttr);
    glVertexAttribPointer(nVUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, uv));

    glEnableVertexAttribArray(nVNormalAttr);
    glVertexAttribPointer(nVNormalAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, normal));
    

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshRenderer::bindVertexArray()
{
    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    
    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexWUVNormal) * m_pMesh->getVertexCount(), m_pMesh->getVertices(), GL_STATIC_DRAW);

    int nFaceCount = static_cast<int>(m_pMesh->getFaces().size());
    m_nVertexCount = nFaceCount * 3;
    glGenBuffers(1, &m_nIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(TriangleFace) * nFaceCount, m_pMesh->getFaces().data(), GL_STATIC_DRAW);

    // Get the attribute locations from the shader
    GLuint nVPosAttr = m_pShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr = m_pShader->getAttributeLocation("a_vUV");
    GLuint nVNormalAttr = m_pShader->getAttributeLocation("a_vNormal");

    // Enable and set the vertex attributes using the retrieved locations
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, pos));

    glEnableVertexAttribArray(nVUVAttr);
    glVertexAttribPointer(nVUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, uv));

    glEnableVertexAttribArray(nVNormalAttr);
    glVertexAttribPointer(nVNormalAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, normal));


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshRenderer::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the mesh");
    ASSERT(m_pMesh, "Mesh must be set before drawing the mesh");


    mat4x4 mvp, local;
    mat4x4_identity(local);
    const Vector3& position = m_pNode->getPosition();
    mat4x4_translate(local, position.x, position.y, position.z);
    mat4x4 rotationMatrix;
    m_pNode->getRotationQuaternion().toMat4x4(rotationMatrix);

    mat4x4_mul(local, local, rotationMatrix);
    // mat4x4_mul(mvp, Camera::main->getViewProjectionMatrix(), local);

    glUseProgram(m_pShader->getProgram());

    glUniformMatrix4fv(m_nModelUniform, 1, GL_FALSE, (const GLfloat*) local);
    glUniformMatrix4fv(m_nViewUniform, 1, GL_FALSE, (const GLfloat*) Camera::main->getViewMatrix());
    glUniformMatrix4fv(m_nProjectionUniform, 1, GL_FALSE, (const GLfloat*) Camera::main->getProjectionMatrix());

    glBindVertexArray(m_nVertexBuffer);
    glDrawElements(GL_TRIANGLES, m_nVertexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    INCREASE_DRAW_CALL_COUNT();

    glUseProgram(0);
}
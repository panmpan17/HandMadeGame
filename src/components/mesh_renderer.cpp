#include "mesh_renderer.h"

#include <glad/gl.h>
#include "../models/simple_obj_reader.h"
#include "../draw/shader.h"
#include "../debug_macro.h"
#include "../window.h"
#include "../node.h"
#include "../camera.h"
#include "../draw/image.h"

MeshRenderer::MeshRenderer(SimpleObjReader* pMesh)
    : m_pMesh(pMesh)
{
}

MeshRenderer::~MeshRenderer()
{
    // if (m_pMesh)
    // {
    //     delete m_pMesh;
    //     m_pMesh = nullptr;
    // }
}

void MeshRenderer::setShader(Shader* pShader)
{
    m_pShader = pShader;

    // m_nMVPUniform = m_pShader->getUniformLocation("u_MVP");
    m_nModelUniform = m_pShader->getUniformLocation("u_Model");
    m_nViewUniform = m_pShader->getUniformLocation("u_View");
    m_nProjectionUniform = m_pShader->getUniformLocation("u_Projection");

    m_pMainTexture = ImageLoader::getInstance()->getImage("box_uv");

    bindVertexArray();
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


    const mat4x4& local = m_pNode->getWorldMatrix();

    glUseProgram(m_pShader->getProgram());

    glUniformMatrix4fv(m_nModelUniform, 1, GL_FALSE, (const GLfloat*) local);
    glUniformMatrix4fv(m_nViewUniform, 1, GL_FALSE, (const GLfloat*) Camera::main->getViewMatrix());
    glUniformMatrix4fv(m_nProjectionUniform, 1, GL_FALSE, (const GLfloat*) Camera::main->getProjectionMatrix());

    if (m_pMainTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_pMainTexture->getTextureID());
        GLuint nMainTexUniform = m_pShader->getUniformLocation("u_MainTex");
        glUniform1i(nMainTexUniform, 0);
    }

    glBindVertexArray(m_nVertexArray);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glDrawElements(GL_TRIANGLES, m_nVertexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    INCREASE_DRAW_CALL_COUNT();

    glUseProgram(0);
}
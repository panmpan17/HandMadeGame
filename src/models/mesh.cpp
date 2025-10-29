#include "mesh.h"

#include <glad/gl.h>


void loadMeshToGPU(Mesh& mesh)
{
    glGenBuffers(1, &mesh.m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_nVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexWUVNormal) * mesh.m_nVertexCount, mesh.m_arrVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.m_nIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.m_nIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.m_nIndiceCount, mesh.m_arrIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

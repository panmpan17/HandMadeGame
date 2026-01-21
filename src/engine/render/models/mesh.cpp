#include "mesh.h"

#include <glad/gl.h>
#include "../../core/window.h"


Mesh::Mesh(int nVertexCount, int nIndiceCount)
        : m_nVertexCount(nVertexCount),
          m_nIndiceCount(nIndiceCount),
          m_nVertexArray(GL_INVALID_VALUE),
          m_nVertexBuffer(GL_INVALID_VALUE),
          m_nIndexBuffer(GL_INVALID_VALUE)
{
    if (m_nVertexCount > 0)
    {
        m_arrVertices = new VertexWUVNormalTangent[m_nVertexCount];
    }
    if (m_nIndiceCount > 0)
    {
        m_arrIndices = new unsigned int[m_nIndiceCount];
    }
}


bool Mesh::getIsGPULoaded() const
{
    return m_nVertexBuffer != GL_INVALID_VALUE && m_nIndexBuffer != GL_INVALID_VALUE;
}


void Mesh::loadToGPU()
{
    if (Window::ins->isUsingMetal())
    {
        glGenBuffers(1, &m_nVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexWUVNormalTangent) * m_nVertexCount, m_arrVertices, GL_STATIC_DRAW);
    
        glGenBuffers(1, &m_nIndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_nIndiceCount, m_arrIndices, GL_STATIC_DRAW);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
    }
#endif // __APPLE__
}

void Mesh::unloadFromGPU()
{
    if (m_nVertexBuffer != 0)
    {
        glDeleteBuffers(1, &m_nVertexBuffer);
        m_nVertexBuffer = 0;
    }

    if (m_nIndexBuffer != 0)
    {
        glDeleteBuffers(1, &m_nIndexBuffer);
        m_nIndexBuffer = 0;
    }

#if __APPLE__
    if (Window::ins->isUsingMetal())
    {
    }
#endif // __APPLE__
}
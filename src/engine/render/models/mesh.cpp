#include "mesh.h"

#include <glad/gl.h>
#include <sstream>
#include "../../core/window.h"


Mesh::Mesh(int nVertexCount, int nIndiceCount)
        : m_nVertexCount(nVertexCount),
          m_nIndiceCount(nIndiceCount),
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
    if (Window::ins->isUsingOpenGL())
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
        MTL::Device* pDevice = Window::ins->getMetalDevice();
        m_pMetalVertexBuffer = pDevice->newBuffer(m_arrVertices, sizeof(VertexWUVNormalTangent) * m_nVertexCount, MTL::ResourceStorageModeShared);

        if (m_nVertexCount < 65536)
        {
            uint16_t* arrIndices16 = new uint16_t[m_nIndiceCount];
            for (size_t i = 0; i < m_nIndiceCount; ++i)
            {
                arrIndices16[i] = static_cast<uint16_t>(m_arrIndices[i]);
            }
            m_pMetalIndexBuffer = pDevice->newBuffer(arrIndices16, sizeof(uint16_t) * m_nIndiceCount, MTL::ResourceStorageModeShared);
            delete[] arrIndices16;

            m_metalIndexType = MTL::IndexTypeUInt16;
        }
        else
        {
            uint32_t* arrIndices32 = new uint32_t[m_nIndiceCount];
            for (size_t i = 0; i < m_nIndiceCount; ++i)
            {
                arrIndices32[i] = static_cast<uint32_t>(m_arrIndices[i]);
            }
            m_pMetalIndexBuffer = pDevice->newBuffer(arrIndices32, sizeof(uint32_t) * m_nIndiceCount, MTL::ResourceStorageModeShared);
            delete[] arrIndices32;

            m_metalIndexType = MTL::IndexTypeUInt32;
        }
    }
#endif // __APPLE__
}

void Mesh::unloadFromGPU()
{
    if (Window::ins->isUsingOpenGL())
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
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        if (m_pMetalVertexBuffer)
        {
            m_pMetalVertexBuffer->release();
            m_pMetalVertexBuffer = nullptr;
        }
    }
#endif // __APPLE__
}
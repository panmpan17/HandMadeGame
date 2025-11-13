#pragma once

#include "../vertex.h"
#include <memory>
#include <glad/gl.h>

typedef unsigned int GLuint;


struct Mesh
{
    GLuint m_nVertexArray = GL_INVALID_VALUE;
    GLuint m_nVertexBuffer = GL_INVALID_VALUE;
    GLuint m_nIndexBuffer = GL_INVALID_VALUE;

    VertexWUVNormalTangent* m_arrVertices = nullptr;
    size_t m_nVertexCount = 0;
    unsigned int* m_arrIndices = nullptr;
    size_t m_nIndiceCount = 0;

    Mesh(int nVertexCount = 0, int nIndiceCount = 0)
        : m_nVertexCount(nVertexCount)
        , m_nIndiceCount(nIndiceCount)
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

    ~Mesh()
    {
        unloadFromCPU();
        unloadFromGPU();
    }

    inline bool getIsGPULoaded() const
    {
        return m_nVertexBuffer != GL_INVALID_VALUE && m_nIndexBuffer != GL_INVALID_VALUE;
    }
    void loadToGPU()
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
    void unloadFromGPU()
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

    inline bool getIsCPULoaded() const
    {
        return m_arrVertices != nullptr && m_arrIndices != nullptr;
    }
    void unloadFromCPU()
    {
        if (m_arrVertices)
        {
            delete[] m_arrVertices;
            m_arrVertices = nullptr;
            m_nVertexCount = 0;
        }

        if (m_arrIndices)
        {
            delete[] m_arrIndices;
            m_arrIndices = nullptr;
            m_nIndiceCount = 0;
        }
    }
};
// void freeMeshFromGPU(Mesh& mesh);

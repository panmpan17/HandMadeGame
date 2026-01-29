#pragma once

#include "../vertex.h"
#include <memory>

#if __APPLE__
#include <Metal/Metal.hpp>
#endif // __APPLE__

typedef unsigned int GLuint;


struct Mesh
{
    GLuint m_nVertexBuffer = 0;
    GLuint m_nIndexBuffer = 0;

#if __APPLE__
    MTL::Buffer* m_pMetalVertexBuffer = nullptr;
    MTL::Buffer* m_pMetalIndexBuffer = nullptr;
    MTL::IndexType m_metalIndexType = MTL::IndexTypeUInt16;
#endif

    VertexWUVNormalTangent* m_arrVertices = nullptr;
    size_t m_nVertexCount = 0;
    unsigned int* m_arrIndices = nullptr;
    size_t m_nIndiceCount = 0;

    Mesh(int nVertexCount = 0, int nIndiceCount = 0);

    ~Mesh()
    {
        unloadFromCPU();
        unloadFromGPU();
    }

    bool getIsGPULoaded() const;

    void loadToGPU();
    void unloadFromGPU();

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

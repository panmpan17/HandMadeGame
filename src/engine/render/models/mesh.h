#pragma once

#include "../vertex.h"
#include <memory>

typedef unsigned int GLuint;


struct Mesh
{
    GLuint m_nVertexArray = 0;
    GLuint m_nVertexBuffer = 0;
    GLuint m_nIndexBuffer = 0;

    VertexWUVNormal* m_arrVertices = nullptr;
    size_t m_nVertexCount = 0;
    unsigned int* m_arrIndices = nullptr;
    size_t m_nIndiceCount = 0;
};

void loadMeshToGPU(Mesh& mesh);
void loadMeshToGPU(const std::shared_ptr<Mesh>& pMesh);
// void freeMeshFromGPU(Mesh& mesh);

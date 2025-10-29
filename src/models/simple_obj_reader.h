#pragma once

#include <string>
#include <vector>
#include "../draw/vertex.h"
#include "mesh.h"

typedef unsigned int GLuint;

class FileReader;
struct Mesh;

struct TriangleFaceVertex
{
    unsigned int m_nVertexIndex;
    unsigned int m_nTexCoordIndex;
    unsigned int m_nNormalIndex;
};

struct TriangleFace
{
   unsigned int v1, v2, v3;
};


class SimpleObjReader
{
public:
    SimpleObjReader(const std::string_view& strFilename);
    ~SimpleObjReader();

    inline const Mesh& getMesh() const { return m_oMesh; }

private:
    FileReader* m_pFileReader = nullptr;

    Mesh m_oMesh;

    void parse();
    int parseFaceVertex(std::vector<TriangleFaceVertex>& vecUniqueVertices, const std::string& strToken);
};
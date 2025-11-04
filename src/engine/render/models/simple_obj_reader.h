#pragma once

#include <string>
#include <vector>
#include "../vertex.h"
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
    SimpleObjReader();
    ~SimpleObjReader();

    std::shared_ptr<Mesh> loadWavefrontFile(const std::string_view& strFilename);

private:
    int parseFaceVertex(std::vector<TriangleFaceVertex>& vecUniqueVertices, const std::string& strToken);
};
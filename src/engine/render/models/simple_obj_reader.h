#pragma once

#include <string>
#include <vector>
#include "mesh.h"
#include "../vertex.h"
#include "../../core/math/vector.h"

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

    std::shared_ptr<Mesh> loadWavefrontFile(std::string_view strFilename);

    static void readVertexBufferFromWavefrontFile(std::string_view strFilename, std::vector<Vector3>& outVertices);

private:
    int parseFaceVertex(std::vector<TriangleFaceVertex>& vecUniqueVertices, const std::string& strToken);
};
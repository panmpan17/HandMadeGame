#pragma once

#include <string>
#include <vector>
#include "../draw/vertex.h"


class FileReader;

struct TriangleFaceVertex
{
    unsigned int m_nVertexIndex;
    unsigned int m_nTexCoordIndex;
    unsigned int m_nNormalIndex;
};

struct TriangleFace
{
    int v1, v2, v3;
};


class SimpleObjReader
{
public:
    SimpleObjReader(const std::string_view& strFilename);
    ~SimpleObjReader();

private:
    VertexWUVNormal* m_pVertices = nullptr;
    size_t m_nVertexCount = 0;
    std::vector<TriangleFace> m_vecFaces;

    FileReader* m_pFileReader = nullptr;

    void parse();
    void loadToGPU();
    int parseFaceVertex(std::vector<TriangleFaceVertex>& vecUniqueVertices, const std::string& strToken);
};
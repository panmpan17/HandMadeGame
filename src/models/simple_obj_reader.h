#pragma once

#include <string>
#include <vector>
#include "../draw/vertex.h"

typedef unsigned int GLuint;

class FileReader;

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

    inline GLuint getVertexArray() const { return m_nVertexArray; }
    // inline size_t getVertexCount() const { return m_nVertexCount; }
    inline size_t getIndexCount() const { return m_vecFaces.size(); }

    inline const VertexWUVNormal* getVertices() const { return m_pVertices; }
    inline size_t getVertexCount() const { return m_nVertexCount; }
    inline const std::vector<TriangleFace>& getFaces() const { return m_vecFaces; }

private:
    VertexWUVNormal* m_pVertices = nullptr;
    size_t m_nVertexCount = 0;
    std::vector<TriangleFace> m_vecFaces;

    FileReader* m_pFileReader = nullptr;

    GLuint m_nVertexBuffer = 0;
    GLuint m_nIndexBuffer = 0;
    GLuint m_nVertexArray = 0;


    void parse();
    void loadToGPU();
    int parseFaceVertex(std::vector<TriangleFaceVertex>& vecUniqueVertices, const std::string& strToken);
};
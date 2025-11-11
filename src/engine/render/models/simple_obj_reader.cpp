#include "simple_obj_reader.h"

#include <linmath.h>
#include <glad/gl.h>
// #include <GLFW/glfw3.h>

#include "../../core/debug_macro.h"
#include "../../core/math/vector.h"
#include "../../../utils/string_handle.h"
#include "../../../utils/file_utils.h"


SimpleObjReader::SimpleObjReader()
{
}

SimpleObjReader::~SimpleObjReader()
{
}

int SimpleObjReader::parseFaceVertex(std::vector<TriangleFaceVertex>& vecUniqueVertices, const std::string& strToken)
{
    std::vector<std::string> parts = splitString(strToken, '/');
    TriangleFaceVertex outTriFaceVertex = { 0, 0, 0 };
    if (parts.size() >= 1 && !parts[0].empty())
    {
        outTriFaceVertex.m_nVertexIndex = std::stoi(parts[0]) - 1; // OBJ indices are 1-based
    }
    if (parts.size() >= 2 && !parts[1].empty())
    {
        outTriFaceVertex.m_nTexCoordIndex = std::stoi(parts[1]) - 1;
    }
    if (parts.size() >= 3 && !parts[2].empty())
    {
        outTriFaceVertex.m_nNormalIndex = std::stoi(parts[2]) - 1;
    }

    // Check if this vertex combination already exists
    for (size_t i = 0; i < vecUniqueVertices.size(); ++i)
    {
        if (vecUniqueVertices[i].m_nVertexIndex == outTriFaceVertex.m_nVertexIndex &&
            vecUniqueVertices[i].m_nTexCoordIndex == outTriFaceVertex.m_nTexCoordIndex &&
            vecUniqueVertices[i].m_nNormalIndex == outTriFaceVertex.m_nNormalIndex)
        {
            return static_cast<int>(i);
        }
    }

    vecUniqueVertices.push_back(outTriFaceVertex);
    return static_cast<int>(vecUniqueVertices.size()) - 1;
}

std::shared_ptr<Mesh> SimpleObjReader::loadWavefrontFile(const std::string_view& strFilename)
{
    auto oFileReader = FileReader(strFilename);
    if (!oFileReader.isOpen())
    {
        LOGLN("Failed to open file: {}", strFilename);
        return nullptr;
    }

    std::vector<Vector3> vecVertices;
    std::vector<Vector2> vecTexCoords;
    std::vector<Vector3> vecNormals;
    vecVertices.reserve(128);
    vecTexCoords.reserve(128);
    vecNormals.reserve(128);

    std::vector<TriangleFaceVertex> vecUniqueVertices;
    vecUniqueVertices.reserve(256);

    std::vector<TriangleFace> vecFaces;
    vecFaces.reserve(64);

    std::string strLine;
    while (oFileReader.readLine(strLine))
    {
        if (strLine.empty())// || strLine[0] == '#')
        {
            continue;
        }

        // TODO: Can be improved with iterator to avoid all string is parse?
        std::vector<std::string> strTokens = splitString(strLine, ' ', true);
        if (strTokens.empty())
            continue;

        int nSize = (int)strTokens.size();
        if (strcmp(strTokens[0].data(), "v") == 0)
        {
            if (nSize < 4)
                continue;

            Vector3 vertex;
            vertex.x = std::stof(strTokens[1]);
            vertex.y = std::stof(strTokens[2]);
            vertex.z = std::stof(strTokens[3]);
            vecVertices.push_back(vertex);
        }
        else if (strcmp(strTokens[0].data(), "vt") == 0)
        {
            if (nSize < 3)
                continue;

            Vector2 texCoord;
            texCoord.x = std::stof(strTokens[1]);
            texCoord.y = std::stof(strTokens[2]);
            vecTexCoords.push_back(texCoord);
        }
        else if (strcmp(strTokens[0].data(), "vn") == 0)
        {
            if (nSize < 4)
                continue;

            Vector3 normal;
            normal.x = std::stof(strTokens[1]);
            normal.y = std::stof(strTokens[2]);
            normal.z = std::stof(strTokens[3]);
            vecNormals.push_back(normal);
        }
        else if (strcmp(strTokens[0].data(), "f") == 0)
        {
            if (nSize == 4)
            {
                TriangleFace oFace;
                oFace.v1 = parseFaceVertex(vecUniqueVertices, strTokens[1]);
                oFace.v2 = parseFaceVertex(vecUniqueVertices, strTokens[2]);
                oFace.v3 = parseFaceVertex(vecUniqueVertices, strTokens[3]);
                vecFaces.push_back(oFace);
            }
            else if (nSize == 5)
            {
                // Quad, need to split into two triangles
                TriangleFace oFace1, oFace2;
                oFace1.v1 = parseFaceVertex(vecUniqueVertices, strTokens[1]);
                oFace1.v2 = parseFaceVertex(vecUniqueVertices, strTokens[2]);
                oFace1.v3 = parseFaceVertex(vecUniqueVertices, strTokens[3]);

                oFace2.v1 = parseFaceVertex(vecUniqueVertices, strTokens[1]);
                oFace2.v2 = parseFaceVertex(vecUniqueVertices, strTokens[3]);
                oFace2.v3 = parseFaceVertex(vecUniqueVertices, strTokens[4]);

                vecFaces.push_back(oFace1);
                vecFaces.push_back(oFace2);
            }
        }
    }

    std::shared_ptr<Mesh> pMesh = std::make_shared<Mesh>();

    pMesh->m_nVertexCount = vecUniqueVertices.size();
    pMesh->m_arrVertices = new VertexWUVNormal[pMesh->m_nVertexCount];
    for (int i = 0; i < pMesh->m_nVertexCount; ++i)
    {
        VertexWUVNormal& oVertex = pMesh->m_arrVertices[i];
        const TriangleFaceVertex& triVert = vecUniqueVertices[i];
        if (triVert.m_nVertexIndex >= 0 && triVert.m_nVertexIndex < vecVertices.size())
        {
            oVertex.pos[0] = vecVertices[triVert.m_nVertexIndex].x;
            oVertex.pos[1] = vecVertices[triVert.m_nVertexIndex].y;
            oVertex.pos[2] = vecVertices[triVert.m_nVertexIndex].z;
        }
        if (triVert.m_nTexCoordIndex >= 0 && triVert.m_nTexCoordIndex < vecTexCoords.size())
        {
            oVertex.uv[0] = vecTexCoords[triVert.m_nTexCoordIndex].x;
            oVertex.uv[1] = vecTexCoords[triVert.m_nTexCoordIndex].y;
        }
        if (triVert.m_nNormalIndex >= 0 && triVert.m_nNormalIndex < vecNormals.size())
        {
            oVertex.normal[0] = vecNormals[triVert.m_nNormalIndex].x;
            oVertex.normal[1] = vecNormals[triVert.m_nNormalIndex].y;
            oVertex.normal[2] = vecNormals[triVert.m_nNormalIndex].z;
        }
    }

    pMesh->m_nIndiceCount = vecFaces.size() * 3;
    pMesh->m_arrIndices = new unsigned int[pMesh->m_nIndiceCount];
    memcpy(pMesh->m_arrIndices, vecFaces.data(), sizeof(unsigned int) * pMesh->m_nIndiceCount);

    loadMeshToGPU(pMesh);

    LOGLN("OBJ parsed: {} vertices, {} tex coords, {} normals, {} unique vertices, {} faces", vecVertices.size(), vecTexCoords.size(), vecNormals.size(), vecUniqueVertices.size(), vecFaces.size());

    return pMesh;
}

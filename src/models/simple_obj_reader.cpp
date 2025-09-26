#include "simple_obj_reader.h"

#include <linmath.h>
#include <glad/gl.h>
// #include <GLFW/glfw3.h>

#include "../file_utils.h"
#include "../debug_macro.h"
#include "../vector.h"
#include "../utils/string_handle.h"


SimpleObjReader::SimpleObjReader(const std::string_view& strFilename)
{
    m_pFileReader = new FileReader(strFilename);
    if (!m_pFileReader->isOpen())
    {
        delete m_pFileReader;
        m_pFileReader = nullptr;
        LOGLN_EX("Failed to open file: {}", strFilename);
        return;
    }

    parse();
    // loadToGPU();
}

SimpleObjReader::~SimpleObjReader()
{
    if (m_pFileReader)
    {
        delete m_pFileReader;
        m_pFileReader = nullptr;
    }
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

void SimpleObjReader::parse()
{
    if (!m_pFileReader)
        return;

    std::vector<Vector3> vecVertices;
    std::vector<Vector2> vecTexCoords;
    std::vector<Vector3> vecNormals;
    vecVertices.reserve(128);
    vecTexCoords.reserve(128);
    vecNormals.reserve(128);

    std::vector<TriangleFaceVertex> vecUniqueVertices;
    vecUniqueVertices.reserve(256);
    m_vecFaces.reserve(64);

    std::string strLine;
    while (m_pFileReader->readLine(strLine))
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
                m_vecFaces.push_back(oFace);
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

                m_vecFaces.push_back(oFace1);
                m_vecFaces.push_back(oFace2);
            }
        }
    }

    m_nVertexCount = vecUniqueVertices.size();
    m_pVertices = new VertexWUVNormal[m_nVertexCount];
    for (int i = 0; i < m_nVertexCount; ++i)
    {
        VertexWUVNormal vertex = {};
        const TriangleFaceVertex& triVert = vecUniqueVertices[i];
        if (triVert.m_nVertexIndex >= 0 && triVert.m_nVertexIndex < vecVertices.size())
        {
            vertex.pos[0] = vecVertices[triVert.m_nVertexIndex].x;
            vertex.pos[1] = vecVertices[triVert.m_nVertexIndex].y;
            vertex.pos[2] = vecVertices[triVert.m_nVertexIndex].z;
        }
        if (triVert.m_nTexCoordIndex >= 0 && triVert.m_nTexCoordIndex < vecTexCoords.size())
        {
            vertex.uv[0] = vecTexCoords[triVert.m_nTexCoordIndex].x;
            vertex.uv[1] = vecTexCoords[triVert.m_nTexCoordIndex].y;
        }
        if (triVert.m_nNormalIndex >= 0 && triVert.m_nNormalIndex < vecNormals.size())
        {
            vertex.normal[0] = vecNormals[triVert.m_nNormalIndex].x;
            vertex.normal[1] = vecNormals[triVert.m_nNormalIndex].y;
            vertex.normal[2] = vecNormals[triVert.m_nNormalIndex].z;
        }

        m_pVertices[i] = vertex;
    }

    LOGLN_EX("OBJ parsed: {} vertices, {} tex coords, {} normals, {} unique vertices, {} faces", vecVertices.size(), vecTexCoords.size(), vecNormals.size(), vecUniqueVertices.size(), m_vecFaces.size());
}

void SimpleObjReader::loadToGPU()
{
    // for (size_t i = 0; i < m_nVertexCount; ++i)
    // {
    //     VertexWUVNormal& vertex = m_pVertices[i];
    //     LOGLN_EX("VertexWUVNormal {}f, {}f, {}f | {}f, {}f | {}f, {}f, {}f", 
    //         vertex.pos[0], vertex.pos[1], vertex.pos[2],
    //         vertex.uv[0], vertex.uv[1],
    //         vertex.normal[0], vertex.normal[1], vertex.normal[2]);
    // }

    // for (size_t i = 0; i < m_vecFaces.size(); ++i)
    // {
    //     TriangleFace& face = m_vecFaces[i];
    //     LOGLN_EX("TriangleFace {}, {}, {},", face.v1, face.v2, face.v3);
    // }

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    
    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexWUVNormal) * m_nVertexCount, m_pVertices, GL_STATIC_DRAW);


    glGenBuffers(1, &m_nIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(TriangleFace) * m_vecFaces.size(), m_vecFaces.data(), GL_STATIC_DRAW);

    // Get the attribute locations from the shader
    // GLuint nVPosAttr = m_pShader->getAttributeLocation("a_vPos");
    // GLuint nVUVAttr = m_pShader->getAttributeLocation("a_vUV");
    // GLuint nVNormalAttr = m_pShader->getAttributeLocation("a_vNormal");

    // Enable and set the vertex attributes using the retrieved locations
    // glEnableVertexAttribArray(nVPosAttr);
    // glVertexAttribPointer(nVPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, pos));

    // glEnableVertexAttribArray(nVUVAttr);
    // glVertexAttribPointer(nVUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, uv));

    // glEnableVertexAttribArray(nVNormalAttr);
    // glVertexAttribPointer(nVNormalAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, normal));


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



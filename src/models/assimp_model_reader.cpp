#include "assimp_model_reader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "../debug_macro.h"
#include "../node.h"
#include "../components/mesh_renderer.h"
#include "../vector.h"

/*
void convertMat4x4(const aiMatrix4x4& aiMat, mat4x4& outMat)
{
    outMat[0][0] = aiMat.a1; outMat[0][1] = aiMat.b1; outMat[0][2] = aiMat.c1; outMat[0][3] = aiMat.d1;
    outMat[1][0] = aiMat.a2; outMat[1][1] = aiMat.b2; outMat[1][2] = aiMat.c2; outMat[1][3] = aiMat.d2;
    outMat[2][0] = aiMat.a3; outMat[2][1] = aiMat.b3; outMat[2][2] = aiMat.c3; outMat[2][3] = aiMat.d3;
    outMat[3][0] = aiMat.a4; outMat[3][1] = aiMat.b4; outMat[3][2] = aiMat.c4; outMat[3][3] = aiMat.d4;
}

void extractPositionFromMatrix(const mat4x4& matrix, Vector3& outPosition, bool bSwapYZ/* = false*)
{
    outPosition.x = matrix[3][0];
    if (bSwapYZ)
    {
        outPosition.y = matrix[3][2];
        outPosition.z = matrix[3][1];
    }
    else
    {
        outPosition.y = matrix[3][1];
        outPosition.z = matrix[3][2];
    }
}

void extractScaleFromMatrix(const mat4x4& matrix, Vector3& outScale)
{
    // Extract scale factors from the columns of the matrix
    outScale.x = sqrtf(matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2]);
    outScale.y = sqrtf(matrix[1][0] * matrix[1][0] + matrix[1][1] * matrix[1][1] + matrix[1][2] * matrix[1][2]);
    outScale.z = sqrtf(matrix[2][0] * matrix[2][0] + matrix[2][1] * matrix[2][1] + matrix[2][2] * matrix[2][2]);
}

void extractRotationMatrixFromMatrix(const mat4x4& matrix, const Vector3& scale, mat4x4& rotationMatrix)
{
    rotationMatrix[0][0] = matrix[0][0] / scale.x;
    rotationMatrix[0][1] = matrix[0][1] / scale.x;
    rotationMatrix[0][2] = matrix[0][2] / scale.x;
    rotationMatrix[0][3] = 0.0f;

    rotationMatrix[1][0] = matrix[1][0] / scale.y;
    rotationMatrix[1][1] = matrix[1][1] / scale.y;
    rotationMatrix[1][2] = matrix[1][2] / scale.y;
    rotationMatrix[1][3] = 0.0f;

    rotationMatrix[2][0] = matrix[2][0] / scale.z;
    rotationMatrix[2][1] = matrix[2][1] / scale.z;
    rotationMatrix[2][2] = matrix[2][2] / scale.z;
    rotationMatrix[2][3] = 0.0f;

    rotationMatrix[3][0] = 0.0f;
    rotationMatrix[3][1] = 0.0f;
    rotationMatrix[3][2] = 0.0f;
    rotationMatrix[3][3] = 1.0f;
}

void rotationMatrixToQuaternion(const mat4x4& rotationMatrix, Quaternion& outQuat)
{
    float trace = rotationMatrix[0][0] + rotationMatrix[1][1] + rotationMatrix[2][2];
    if (trace > 0.0f)
    {
        float s = sqrtf(trace + 1.0f) * 2.0f; // S=4*qw
        outQuat.w = 0.25f * s;
        outQuat.x = (rotationMatrix[2][1] - rotationMatrix[1][2]) / s;
        outQuat.y = (rotationMatrix[0][2] - rotationMatrix[2][0]) / s;
        outQuat.z = (rotationMatrix[1][0] - rotationMatrix[0][1]) / s;
    }
    else if (rotationMatrix[0][0] > rotationMatrix[1][1] && rotationMatrix[0][0] > rotationMatrix[2][2])
    {
        float s = sqrtf(1.0f + rotationMatrix[0][0] - rotationMatrix[1][1] - rotationMatrix[2][2]) * 2.0f; // S=4*qx
        outQuat.w = (rotationMatrix[2][1] - rotationMatrix[1][2]) / s;
        outQuat.x = 0.25f * s;
        outQuat.y = (rotationMatrix[0][1] + rotationMatrix[1][0]) / s;
        outQuat.z = (rotationMatrix[0][2] + rotationMatrix[2][0]) / s;
    }
    else if (rotationMatrix[1][1] > rotationMatrix[2][2])
    {
        float s = sqrtf(1.0f + rotationMatrix[1][1] - rotationMatrix[0][0] - rotationMatrix[2][2]) * 2.0f; // S=4*qy
        outQuat.w = (rotationMatrix[0][2] - rotationMatrix[2][0]) / s;
        outQuat.x = (rotationMatrix[0][1] + rotationMatrix[1][0]) / s;
        outQuat.y = 0.25f * s;
        outQuat.z = (rotationMatrix[1][2] + rotationMatrix[2][1]) / s;
    }
    else
    {
        float s = sqrtf(1.0f + rotationMatrix[2][2] - rotationMatrix[0][0] - rotationMatrix[1][1]) * 2.0f; // S=4*qz
        outQuat.w = (rotationMatrix[1][0] - rotationMatrix[0][1]) / s;
        outQuat.x = (rotationMatrix[0][2] + rotationMatrix[2][0]) / s;
        outQuat.y = (rotationMatrix[1][2] + rotationMatrix[2][1]) / s;
        outQuat.z = 0.25f * s;
    }
}
*/

Node* loadModel(const std::string_view& strPath, Shader* pShader)
{
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(strPath.data(),
                                              aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) 
    {
        LOGLN_EX( "Error loading model: {}", importer.GetErrorString() );
        return nullptr;
    }

    LOGLN_EX( "Model {} loaded successfully with {} meshes.", strPath, pScene->mNumMeshes);

    return processNode(pScene->mRootNode, pScene, pShader);
}

Node* processNode(const aiNode* pAiNode, const aiScene* pScene, Shader* pShader)
{
    aiVector3D scale;
    aiVector3D position;
    aiQuaternion rotationQuatAi;
    // This should extract scale first, then rotation, then position
    pAiNode->mTransformation.Decompose(scale, rotationQuatAi, position);

    Node* pNode = new Node();
    pNode->setPosition(position.x, position.y, position.z);
    pNode->setScale(scale.x, scale.y, scale.z);
    Quaternion rotationQuat(rotationQuatAi.w, rotationQuatAi.x, rotationQuatAi.y, rotationQuatAi.z);
    pNode->setRotationQuaternion(rotationQuat);

    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < pAiNode->mNumMeshes; i++)
    {
        auto pMeshRenderer = new MeshRenderer();
        pMeshRenderer->setMesh(processMesh(pScene->mMeshes[pAiNode->mMeshes[i]]));
        pMeshRenderer->setShader(pShader);
        pNode->addComponent(pMeshRenderer);
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < pAiNode->mNumChildren; i++)
    {
        Node* pChildNod = processNode(pAiNode->mChildren[i], pScene, pShader);
        pNode->addChildNode(pChildNod);
    }

    return pNode;
}

std::shared_ptr<Mesh> processMesh(const aiMesh* pAiMesh)
{
    std::shared_ptr<Mesh> pMesh = std::make_shared<Mesh>();

    pMesh->m_nVertexCount = pAiMesh->mNumVertices;
    pMesh->m_arrVertices = new VertexWUVNormal[pMesh->m_nVertexCount];
    for (unsigned int i = 0; i < pAiMesh->mNumVertices; ++i)
    {
        VertexWUVNormal& oVertex = pMesh->m_arrVertices[i];
        const aiVector3D& aiPos = pAiMesh->mVertices[i];
        oVertex.pos[0] = aiPos.x;
        oVertex.pos[1] = aiPos.y;
        oVertex.pos[2] = aiPos.z;
        if (pAiMesh->mNormals)
        {
            const aiVector3D& aiNormal = pAiMesh->mNormals[i];
            oVertex.normal[0] = aiNormal.x;
            oVertex.normal[1] = aiNormal.y;
            oVertex.normal[2] = aiNormal.z;
        }
        else
        {
            oVertex.normal[0] = 0.0f;
            oVertex.normal[1] = 0.0f;
            oVertex.normal[2] = 0.0f;
        }
        if (pAiMesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            const aiVector3D& aiTexCoord = pAiMesh->mTextureCoords[0][i];
            oVertex.uv[0] = aiTexCoord.x;
            oVertex.uv[1] = aiTexCoord.y;
        }
        else
        {
            oVertex.uv[0] = 0.0f;
            oVertex.uv[1] = 0.0f;
        }
    }

    pMesh->m_nIndiceCount = pAiMesh->mNumFaces * 3;
    pMesh->m_arrIndices = new unsigned int[pMesh->m_nIndiceCount];
    unsigned int index = 0;
    for (unsigned int i = 0; i < pAiMesh->mNumFaces; ++i)
    {
        const aiFace& face = pAiMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            pMesh->m_arrIndices[index++] = face.mIndices[j];
        }
    }

    loadMeshToGPU(pMesh);

    return pMesh;
}
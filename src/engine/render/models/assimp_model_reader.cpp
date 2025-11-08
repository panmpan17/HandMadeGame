#include "assimp_model_reader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "../image.h"
#include "../material.h"
#include "../../core/debug_macro.h"
#include "../../core/scene/node.h"
#include "../../core/math/vector.h"
#include "../../components/render/mesh_renderer.h"
#include "../../../utils/file_utils.h"
#include "../../../utils/filesystem.h"

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

Node* loadModel(const std::string_view& strPath, std::shared_ptr<Material>& pMaterial)
{
    Assimp::Importer importer;

    const aiScene* pScene;

    if (*strPath.begin() != '/')
    {
        std::string strFullPath = fs::path(FileUtils::getResourcesPath()).append(strPath).string();
        pScene = importer.ReadFile(strFullPath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    }
    else
    {
        pScene = importer.ReadFile(strPath.data(), aiProcess_Triangulate | aiProcess_FlipUVs);
    }

    if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) 
    {
        LOGLN( "Error loading model: {}", importer.GetErrorString() );
        return nullptr;
    }

    LOGLN( "Model {} loaded successfully with {} meshes.", strPath, pScene->mNumMeshes);

    // for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
    // {
    //     const aiMaterial* const pMaterial = pScene->mMaterials[i];
    //     int nDiffuseTexCount = pMaterial->GetTextureCount(aiTextureType_DIFFUSE);
    //     LOGLN("Material {} has {} DIFFUSE, {} SPECULAR, {} AMBIENT, {} EMISSIVE, {} HEIGHT, {} NORMALS, {} SHININESS, {} OPACITY, {} DISPLACEMENT, {} LIGHTMAP, {} REFLECTION, {} BASE_COLOR, {} NORMAL_CAMERA, {} EMISSION_COLOR, {} METALNESS, {} DIFFUSE_ROUGHNESS, {} AMBIENT_OCCLUSION, {} UNKNOWN, {} SHEEN, {} CLEARCOAT, {} TRANSMISSION, {} MAYA_BASE, {} MAYA_SPECULAR, {} MAYA_SPECULAR_COLOR, {} MAYA_SPECULAR_ROUGHNESS, {} ANISOTROPY, {} GLTF_METALLIC_ROUGHNES",
    //         pMaterial->GetName().C_Str(),
    //         pMaterial->GetTextureCount(aiTextureType_DIFFUSE),
    //         pMaterial->GetTextureCount(aiTextureType_SPECULAR),
    //         pMaterial->GetTextureCount(aiTextureType_AMBIENT),
    //         pMaterial->GetTextureCount(aiTextureType_EMISSIVE),
    //         pMaterial->GetTextureCount(aiTextureType_HEIGHT),
    //         pMaterial->GetTextureCount(aiTextureType_NORMALS),
    //         pMaterial->GetTextureCount(aiTextureType_SHININESS),
    //         pMaterial->GetTextureCount(aiTextureType_OPACITY),
    //         pMaterial->GetTextureCount(aiTextureType_DISPLACEMENT),
    //         pMaterial->GetTextureCount(aiTextureType_LIGHTMAP),
    //         pMaterial->GetTextureCount(aiTextureType_REFLECTION),
    //         pMaterial->GetTextureCount(aiTextureType_BASE_COLOR),
    //         pMaterial->GetTextureCount(aiTextureType_NORMAL_CAMERA),
    //         pMaterial->GetTextureCount(aiTextureType_EMISSION_COLOR),
    //         pMaterial->GetTextureCount(aiTextureType_METALNESS),
    //         pMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS),
    //         pMaterial->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION),
    //         pMaterial->GetTextureCount(aiTextureType_UNKNOWN),
    //         pMaterial->GetTextureCount(aiTextureType_SHEEN),
    //         pMaterial->GetTextureCount(aiTextureType_CLEARCOAT),
    //         pMaterial->GetTextureCount(aiTextureType_TRANSMISSION),
    //         pMaterial->GetTextureCount(aiTextureType_MAYA_BASE),
    //         pMaterial->GetTextureCount(aiTextureType_MAYA_SPECULAR),
    //         pMaterial->GetTextureCount(aiTextureType_MAYA_SPECULAR_COLOR),
    //         pMaterial->GetTextureCount(aiTextureType_MAYA_SPECULAR_ROUGHNESS),
    //         pMaterial->GetTextureCount(aiTextureType_ANISOTROPY),
    //         pMaterial->GetTextureCount(aiTextureType_GLTF_METALLIC_ROUGHNESS)
    //         );
    //     for (unsigned int j = 0; j < nDiffuseTexCount; ++j)
    //     {
    //         aiString strTexturePath;
    //         if (pMaterial->GetTexture(aiTextureType_DIFFUSE, j, &strTexturePath) == AI_SUCCESS)
    //         {
    //             LOGLN("Material {} has diffuse texture: {}", i, strTexturePath.C_Str());
    //         }
    //     }
    // }

    return processNode(pScene->mRootNode, pScene, pMaterial);
}

Node* processNode(const aiNode* pAiNode, const aiScene* pScene, std::shared_ptr<Material>& pMaterial)
{
    aiVector3D scale;
    aiVector3D position;
    aiQuaternion rotationQuatAi;
    // This should extract scale first, then rotation, then position
    pAiNode->mTransformation.Decompose(scale, rotationQuatAi, position);

    Node* pNode = new Node();
    pNode->setName(pAiNode->mName.C_Str());
    pNode->setPosition(position.x, position.y, position.z);
    pNode->setScale(scale.x, scale.y, scale.z);
    Quaternion rotationQuat(rotationQuatAi.w, rotationQuatAi.x, rotationQuatAi.y, rotationQuatAi.z);
    pNode->setRotationQuaternion(rotationQuat);

    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < pAiNode->mNumMeshes; i++)
    {
        auto pMeshRenderer = new MeshRenderer();
        pMeshRenderer->setMesh(processMesh(pScene->mMeshes[pAiNode->mMeshes[i]], pScene));
        pMeshRenderer->setMaterial(pMaterial);
        pNode->addComponent(pMeshRenderer);
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < pAiNode->mNumChildren; i++)
    {
        Node* pChildNod = processNode(pAiNode->mChildren[i], pScene, pMaterial);
        pNode->addChildNode(pChildNod);
    }

    return pNode;
}

std::shared_ptr<Mesh> processMesh(const aiMesh* pAiMesh, const aiScene* pScene)
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
            oVertex.uv[1] = 1.0f - aiTexCoord.y;
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
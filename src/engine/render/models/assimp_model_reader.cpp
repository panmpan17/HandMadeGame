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

    std::vector<std::shared_ptr<Material>> arrMaterials;
    for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
    {
        std::shared_ptr<Material> pNewMaterial = std::make_shared<Material>(pMaterial->getShader());

        const aiMaterial* const pAiMaterial = pScene->mMaterials[i];

        // LOGLN("Material {} has {} DIFFUSE, {} SPECULAR, {} AMBIENT, {} EMISSIVE, {} HEIGHT, {} NORMALS, {} SHININESS, {} OPACITY, {} DISPLACEMENT, {} LIGHTMAP, {} REFLECTION, {} BASE_COLOR, {} NORMAL_CAMERA, {} EMISSION_COLOR, {} METALNESS, {} DIFFUSE_ROUGHNESS, {} AMBIENT_OCCLUSION, {} UNKNOWN, {} SHEEN, {} CLEARCOAT, {} TRANSMISSION, {} MAYA_BASE, {} MAYA_SPECULAR, {} MAYA_SPECULAR_COLOR, {} MAYA_SPECULAR_ROUGHNESS, {} ANISOTROPY, {} GLTF_METALLIC_ROUGHNES",
        //     pAiMaterial->GetName().C_Str(),
        //     pAiMaterial->GetTextureCount(aiTextureType_DIFFUSE),
        //     pAiMaterial->GetTextureCount(aiTextureType_SPECULAR),
        //     pAiMaterial->GetTextureCount(aiTextureType_AMBIENT),
        //     pAiMaterial->GetTextureCount(aiTextureType_EMISSIVE),
        //     pAiMaterial->GetTextureCount(aiTextureType_HEIGHT),
        //     pAiMaterial->GetTextureCount(aiTextureType_NORMALS),
        //     pAiMaterial->GetTextureCount(aiTextureType_SHININESS),
        //     pAiMaterial->GetTextureCount(aiTextureType_OPACITY),
        //     pAiMaterial->GetTextureCount(aiTextureType_DISPLACEMENT),
        //     pAiMaterial->GetTextureCount(aiTextureType_LIGHTMAP),
        //     pAiMaterial->GetTextureCount(aiTextureType_REFLECTION),
        //     pAiMaterial->GetTextureCount(aiTextureType_BASE_COLOR),
        //     pAiMaterial->GetTextureCount(aiTextureType_NORMAL_CAMERA),
        //     pAiMaterial->GetTextureCount(aiTextureType_EMISSION_COLOR),
        //     pAiMaterial->GetTextureCount(aiTextureType_METALNESS),
        //     pAiMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS),
        //     pAiMaterial->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION),
        //     pAiMaterial->GetTextureCount(aiTextureType_UNKNOWN),
        //     pAiMaterial->GetTextureCount(aiTextureType_SHEEN),
        //     pAiMaterial->GetTextureCount(aiTextureType_CLEARCOAT),
        //     pAiMaterial->GetTextureCount(aiTextureType_TRANSMISSION),
        //     pAiMaterial->GetTextureCount(aiTextureType_MAYA_BASE),
        //     pAiMaterial->GetTextureCount(aiTextureType_MAYA_SPECULAR),
        //     pAiMaterial->GetTextureCount(aiTextureType_MAYA_SPECULAR_COLOR),
        //     pAiMaterial->GetTextureCount(aiTextureType_MAYA_SPECULAR_ROUGHNESS),
        //     pAiMaterial->GetTextureCount(aiTextureType_ANISOTROPY),
        //     pAiMaterial->GetTextureCount(aiTextureType_GLTF_METALLIC_ROUGHNESS)
        //     );

        int nDiffuseTexCount = pAiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
        for (unsigned int j = 0; j < nDiffuseTexCount; ++j)
        {
            aiString strTexturePath;
            if (pAiMaterial->GetTexture(aiTextureType_DIFFUSE, j, &strTexturePath) == AI_SUCCESS)
            {
                const aiTexture* pAiTexture = pScene->GetEmbeddedTexture(strTexturePath.C_Str());
                Image* pDiffuseImage = new Image(pAiTexture);
                pDiffuseImage->loadTextureToGL();
                pNewMaterial->setAlbedoMap(pDiffuseImage);
            }
        }

        int nNormalTexCount = pAiMaterial->GetTextureCount(aiTextureType_NORMALS);
        for (unsigned int j = 0; j < nNormalTexCount; ++j)
        {
            aiString strTexturePath;
            if (pAiMaterial->GetTexture(aiTextureType_NORMALS, j, &strTexturePath) == AI_SUCCESS)
            {
                const aiTexture* pAiTexture = pScene->GetEmbeddedTexture(strTexturePath.C_Str());
                Image* pNormalImage = new Image(pAiTexture);
                pNormalImage->loadTextureToGL();
                pNewMaterial->setNormalMap(pNormalImage);
            }
        }

        arrMaterials.push_back(pNewMaterial);
    }


    return processNode(pScene->mRootNode, pScene, arrMaterials, pMaterial);
}

Node* processNode(const aiNode* pAiNode, const aiScene* pScene, std::vector<std::shared_ptr<Material>>& vecSceneMaterials, std::shared_ptr<Material>& pMaterial)
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

        unsigned int nMaterialIndex = pScene->mMeshes[pAiNode->mMeshes[i]]->mMaterialIndex;
        if (nMaterialIndex < vecSceneMaterials.size())
        {
            LOGLN( "Assigning material index {} to mesh.", nMaterialIndex );
            pMeshRenderer->setMaterial(vecSceneMaterials.at(nMaterialIndex));
        }
        else
        {
            pMeshRenderer->setMaterial(pMaterial);
        }
        pNode->addComponent(pMeshRenderer);
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < pAiNode->mNumChildren; i++)
    {
        Node* pChildNod = processNode(pAiNode->mChildren[i], pScene, vecSceneMaterials, pMaterial);
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
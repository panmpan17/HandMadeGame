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


Node* AssimpModelReader::loadModel()
{
    // PROFILER_START_TIMER();

    Assimp::Importer importer;

    if (*m_strPath.begin() != '/')
    {
        std::string strFullPath = fs::path(FileUtils::getResourcesPath()).append(m_strPath).string();
        m_pScene = importer.ReadFile(strFullPath.c_str(),
                                   aiProcess_Triangulate | aiProcess_FlipUVs
                                   | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
    }
    else
    {
        m_pScene = importer.ReadFile(m_strPath.data(),
                                   aiProcess_Triangulate | aiProcess_FlipUVs
                                   | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
    }

    // PROFILER_END_TIMER("Assimp Importer ReadFile");

    if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode) 
    {
        LOGLN( "Error loading model: {}", importer.GetErrorString() );
        return nullptr;
    }

    /*
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
    */


    m_pRootNode = processNode(m_pScene->mRootNode);
    // PROFILER_END_TIMER("Process root node done");
    return m_pRootNode;
}

Node* AssimpModelReader::processNode(const aiNode* pAiNode)
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
        pMeshRenderer->setMesh(processMesh(m_pScene->mMeshes[pAiNode->mMeshes[i]]));

        unsigned int nMaterialIndex = m_pScene->mMeshes[pAiNode->mMeshes[i]]->mMaterialIndex;
        if (nMaterialIndex < m_vecOverrideMaterials.size())
        {
            pMeshRenderer->setMaterial(m_vecOverrideMaterials.at(nMaterialIndex));
        }
        else if (nMaterialIndex < m_vecSceneMaterials.size())
        {
            pMeshRenderer->setMaterial(m_vecSceneMaterials.at(nMaterialIndex));
        }

        m_mapMeshToMaterialIndex[pMeshRenderer] = nMaterialIndex;
        pNode->addComponent(pMeshRenderer);
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < pAiNode->mNumChildren; i++)
    {
        Node* pChildNod = processNode(pAiNode->mChildren[i]);
        pNode->addChildNode(pChildNod);
    }

    return pNode;
}

std::shared_ptr<Mesh> AssimpModelReader::processMesh(const aiMesh* pAiMesh)
{
    std::shared_ptr<Mesh> pMesh = std::make_shared<Mesh>(pAiMesh->mNumVertices, pAiMesh->mNumFaces * 3);

    for (unsigned int i = 0; i < pAiMesh->mNumVertices; ++i)
    {
        VertexWUVNormalTangent& oVertex = pMesh->m_arrVertices[i];
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

        if (pAiMesh->mTangents)
        {
            const aiVector3D& aiTangent = pAiMesh->mTangents[i];
            oVertex.tangent[0] = aiTangent.x;
            oVertex.tangent[1] = aiTangent.y;
            oVertex.tangent[2] = aiTangent.z;
        }
        else
        {
            oVertex.tangent[0] = 0.0f;
            oVertex.tangent[1] = 0.0f;
            oVertex.tangent[2] = 0.0f;
        }

        // Bitangents can be computed if needed
        if (pAiMesh->mBitangents)
        {
            const aiVector3D& aiBitangent = pAiMesh->mBitangents[i];
            oVertex.bitangent[0] = aiBitangent.x;
            oVertex.bitangent[1] = aiBitangent.y;
            oVertex.bitangent[2] = aiBitangent.z;
        }
        else
        {
            oVertex.bitangent[0] = 0.0f;
            oVertex.bitangent[1] = 0.0f;
            oVertex.bitangent[2] = 0.0f;
        }
    }

    unsigned int index = 0;
    for (unsigned int i = 0; i < pAiMesh->mNumFaces; ++i)
    {
        const aiFace& face = pAiMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            pMesh->m_arrIndices[index++] = face.mIndices[j];
        }
    }

    pMesh->loadToGPU();

    return pMesh;
}

Node* AssimpModelReader::instantiateCloneNode() const
{
    if (m_pRootNode)
    {
        return m_pRootNode->clone();
    }
    return nullptr;
}

Node* AssimpModelReader::instantiateCloneNode(const std::vector<std::shared_ptr<Material>>& vecOverrideMaterials)
{
    m_vecOverrideMaterials = vecOverrideMaterials;

    if (m_pRootNode)
    {
        Node* pClonedNode = m_pRootNode->clone();
        overrideCloneNodeMaterials(m_pRootNode, pClonedNode, vecOverrideMaterials);
        return pClonedNode;
    }

    return nullptr;
}

void AssimpModelReader::overrideCloneNodeMaterials(Node* pOriginalNode, Node* pClonedNode, const std::vector<std::shared_ptr<Material>>& vecOverrideMaterials)
{
    int nSize = pClonedNode->getComponentCount();
    for (int i = 0; i < nSize; ++i)
    {
        MeshRenderer* pOriginalMeshRenderer = dynamic_cast<MeshRenderer*>(pOriginalNode->getComponent(i));
        if (pOriginalMeshRenderer)
        {
            MeshRenderer* pMeshRenderer = static_cast<MeshRenderer*>(pClonedNode->getComponent(i));
            auto it = m_mapMeshToMaterialIndex.find(pOriginalMeshRenderer);
            if (it != m_mapMeshToMaterialIndex.end())
            {
                unsigned int nMaterialIndex = it->second;
                if (nMaterialIndex < vecOverrideMaterials.size())
                {
                    pMeshRenderer->setMaterial(vecOverrideMaterials.at(nMaterialIndex));
                }
            }
        }
    }

    nSize = pClonedNode->getChildNodeCount();
    for (int i = 0; i < nSize; ++i)
    {
        overrideCloneNodeMaterials(pOriginalNode->getChildNode(i), pClonedNode->getChildNode(i), vecOverrideMaterials);
    }
}

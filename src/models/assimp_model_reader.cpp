#include "assimp_model_reader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../debug_macro.h"
#include "../node.h"
#include "../components/mesh_renderer.h"
// #include "../draw/shader.h"


void extractPositionFromMatrix(const aiMatrix4x4& matrix, float& x, float& y, float& z)
{
    x = matrix.a4;
    y = matrix.b4;
    z = matrix.c4;
}



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
    // float posX, posY, posZ;
    // extractPositionFromMatrix(pAiNode->mTransformation, posX, posY, posZ);
    Node* pNode = new Node(0, 0, 0);

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
#include "assimp_model_reader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../debug_macro.h"


void loadModel(const std::string_view& strPath)
{
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(strPath.data(),
                                              aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) 
    {
        LOGLN_EX( "Error loading model: {}", importer.GetErrorString() );
        return;
    }

    LOGLN_EX( "Model {} loaded successfully with {} meshes.", strPath, pScene->mNumMeshes );

    processNode(pScene->mRootNode, pScene);
}

void processNode(const aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        const aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        LOGLN_EX( "Processing mesh {} with {} vertices and {} faces.", i, mesh->mNumVertices, mesh->mNumFaces );
        // TODO: process mesh into my own format
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
} 
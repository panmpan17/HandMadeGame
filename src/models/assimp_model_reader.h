#pragma once

#include <string_view>
#include <memory>
#include "mesh.h"
// #include "../node.h"
// #include "../src/components/mesh_renderer.h"

struct aiScene;
struct aiNode;
struct aiMesh;
// struct aiMaterial;

class Node;
class Shader;


Node* loadModel(const std::string_view& strPath, Shader* pShader);

Node* processNode(const aiNode* pAiNode, const aiScene* pScene, Shader* pShader);

std::shared_ptr<Mesh> processMesh(const aiMesh* pMesh);


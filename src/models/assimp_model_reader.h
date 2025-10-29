#pragma once

#include <string_view>

struct aiScene;
struct aiNode;
// struct aiMaterial;
// struct aiMesh;

void loadModel(const std::string_view& strPath);

void processNode(const aiNode* pNode, const aiScene* pScene);

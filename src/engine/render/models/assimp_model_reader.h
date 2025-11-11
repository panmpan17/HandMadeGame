#pragma once

#include <string_view>
#include <memory>
#include "mesh.h"
#include <linmath.h>
#include <vector>
// #include "../node.h"
// #include "../src/components/mesh_renderer.h"

struct aiScene;
struct aiNode;
struct aiMesh;
// struct aiMaterial;
// struct aiMatrix4x4;

class Node;
class Shader;
struct Quaternion;
struct Vector3;
class Image;
class Material;


// void extractPositionFromMatrix(const mat4x4& matrix, Vector3& outPosition, bool bSwapYZ = false);
// void extractScaleFromMatrix(const mat4x4& matrix, Vector3& outScale);

// void rotationMatrixToQuaternion(const mat4x4& rotationMatrix, Quaternion& outQuat);
// void extractRotationMatrixFromMatrix(const mat4x4& matrix, const Vector3& scale, mat4x4& rotationMatrix);

Node* loadModel(const std::string_view& strPath, std::shared_ptr<Material>& pMaterial);

Node* processNode(const aiNode* pAiNode, const aiScene* pScene, std::vector<std::shared_ptr<Material>>& vecSceneMaterials, std::shared_ptr<Material>& pMaterial);

std::shared_ptr<Mesh> processMesh(const aiMesh* pMesh, const aiScene* pScene);


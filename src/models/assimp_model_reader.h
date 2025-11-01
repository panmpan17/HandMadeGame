#pragma once

#include <string_view>
#include <memory>
#include "mesh.h"
#include <linmath.h>
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


void extractPositionFromMatrix(const mat4x4& matrix, float& x, float& y, float& z);
void extractScaleFromMatrix(const mat4x4& matrix, Vector3& outScale);

void rotationMatrixToQuaternion(const mat4x4& rotationMatrix, Quaternion& outQuat);
void extractRotationMatrixFromMatrix(const mat4x4& matrix, const Vector3& scale, mat4x4& rotationMatrix);

Node* loadModel(const std::string_view& strPath, Shader* pShader);

Node* processNode(const aiNode* pAiNode, const aiScene* pScene, Shader* pShader);

std::shared_ptr<Mesh> processMesh(const aiMesh* pMesh);


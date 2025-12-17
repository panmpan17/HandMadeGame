#pragma once

#include <string_view>
#include <memory>
#include <linmath.h>
#include <unordered_map>
#include <vector>
#include "mesh.h"

struct aiScene;
struct aiNode;
struct aiMesh;

class Node;
class Material;
class MeshRenderer;


class AssimpModelReader
{
public:
    AssimpModelReader(const std::string& strPath) : m_strPath(strPath) { loadModel(); }
    AssimpModelReader(const std::string& strPath, const std::vector<std::shared_ptr<Material>>& vecOverrideMaterials) :
        m_strPath(strPath), m_vecOverrideMaterials(vecOverrideMaterials) { loadModel(); }
    ~AssimpModelReader();

    Node* instantiateCloneNode() const;
    Node* instantiateCloneNode(const std::vector<std::shared_ptr<Material>>& vecOverrideMaterials);

private:
    std::string m_strPath;
    std::vector<std::shared_ptr<Material>> m_vecOverrideMaterials;
    std::vector<std::shared_ptr<Material>> m_vecSceneMaterials;
    std::unordered_map<MeshRenderer*, unsigned int> m_mapMeshToMaterialIndex;

    Node* m_pRootNode = nullptr; // Should be destroyed when reader is destroyed
    const aiScene* m_pScene = nullptr;

    std::shared_ptr<Material> m_pDefaultMaterial = nullptr;
    std::shared_ptr<Material> getDefaultMaterial();

    void loadModel();
    void loadSceneMaterials();

    Node* processNode(const aiNode* pAiNode);
    std::shared_ptr<Mesh> processMesh(const aiMesh* pMesh);

    void overrideCloneNodeMaterials(Node* pOriginalNode, Node* pClonedNode, const std::vector<std::shared_ptr<Material>>& vecOverrideMaterials);
};

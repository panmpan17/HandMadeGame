#pragma once

#include "../../../utils/expandable_array.h"
#include "node.h"

class Node;
class Skybox;

class WorldScene {
public:
    static WorldScene* current;

    WorldScene();
    ~WorldScene();

    void init();
    void createPinPongGame();
    void bloomTest();
    void readFromFiles(const std::string_view& strFilePath);

    void addNode(Node* pNode);
    void clearAllNodes();

    void onStart();

    void update(float fDeltatime);
    void render();

    inline int getNodeCount() const { return m_oNodeArray.getSize(); }
    inline Node* getNode(int nIndex) const { return m_oNodeArray.getElement(nIndex); }

    Skybox* getSkybox() const { return m_pSkybox; }

private:
    PointerExpandableArray<Node*> m_oNodeArray = PointerExpandableArray<Node*>(10);

    Skybox* m_pSkybox = nullptr;
};

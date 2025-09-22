#pragma once

#include "expandable_array.h"

class Node;

class WorldScene {
public:
    WorldScene();
    ~WorldScene();

    void init();
    void createPinPongGame();
    void bloomTest();
    void readFromFiles(const std::string_view& strFilePath);

    void addNode(Node* pNode);
    void clearAllNodes();

    void update(float fDeltatime);
    void render();

private:
    PointerExpandableArray<Node*> m_oNodeArray = PointerExpandableArray<Node*>(10);
};

#pragma once

#include "expandable_array.h"

class Node;

class WorldScene {
public:
    WorldScene();
    ~WorldScene();

    void init();
    void update(float fDeltatime);
    void render();

    void addNode(Node* pNode);

    void readFromFiles(const std::string_view& strFilePath);

private:
    PointerExpandableArray<Node*> m_oNodeArray = PointerExpandableArray<Node*>(10);
};

#pragma once

#include "expandable_array.h"

class Node;
class TestShader;
class ImageShader;

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
    TestShader* m_pBaseShader = nullptr;
    ImageShader* m_pImageShader = nullptr;

    PointerExpandableArray<Node*> m_oNodeArray = PointerExpandableArray<Node*>(10);
};

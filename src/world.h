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

private:
    TestShader* m_pBaseShader = nullptr;
    ImageShader* m_pImageShader = nullptr;

    PointerExtendableArray<Node*> m_oNodeArray = PointerExtendableArray<Node*>(10);
};

#pragma once

#include "../../../utils/expandable_array.h"
#include "node.h"
#include "../../components/drawable_interface.h"

class Node;
class IDrawable;
// class Skybox;

class WorldScene {
public:
    static WorldScene* current;

    WorldScene();
    ~WorldScene();

    void init();
    void createPinPongGame();
    void readFromFiles(const std::string_view& strFilePath);

    void addNode(Node* pNode);
    void clearAllNodes();

    void addDrawable(IDrawable* const pDrawable);
    void removeDrawable(IDrawable* const pDrawable);

    void update(float fDeltatime);
    void render();
    void renderDepth();
    void drawGizmos();

    inline int getNodeCount() const { return m_oNodeArray.getCount(); }
    inline Node* getNode(int nIndex) const { return m_oNodeArray.getElement(nIndex); }

    // inline Skybox* getSkybox() const { return m_pSkybox; }
    // inline void setSkybox(Skybox* pSkybox) { m_pSkybox = pSkybox; }

    inline void setInitEditorCamera(bool bInit) { m_bInitEditorCamera = bInit; }

private:
    PointerExpandableArray<Node*> m_oNodeArray = PointerExpandableArray<Node*>(16);
    PointerExpandableArray<IDrawable*> m_oOpaqueDrawableArray = PointerExpandableArray<IDrawable*>(16);
    PointerExpandableArray<IDrawable*> m_oTransparentDrawableArray = PointerExpandableArray<IDrawable*>(16);

    // Skybox* m_pSkybox = nullptr;

    bool m_bInitEditorCamera = true;
};

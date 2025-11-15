#pragma once

#include "editor_window.h"


class Node;
class Component;


class NodeInspector : public IEditorWindow
{
public:
    NodeInspector();
    ~NodeInspector();

    void update() override;

private:
    bool m_bCollapsed = false;

    void updateTransform(Node* pNode);
    void updateComponent(Node* pNode, Component* pComponent);
};

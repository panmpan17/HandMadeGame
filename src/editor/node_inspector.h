#pragma once

#include "editor_window.h"


class Node;
class NodeComponent;


class NodeInspector : public IEditorWindow
{
public:
    NodeInspector();
    ~NodeInspector();

    void update() override;

private:
    bool m_bCollapsed = false;

    void updateTransform(Node* pNode);
    void updateComponent(int nComponentIndex, Node* pNode, NodeComponent* pComponent);
};

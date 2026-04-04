#pragma once


#include "editor_window.h"
#include <string>
#include <vector>


class Node;


class HierarchyView : public IEditorWindow
{
public:
    HierarchyView();
    ~HierarchyView();

    void update() override;

private:
    bool m_bCollapsed = false;

    std::vector<Node*> m_vecHideChildrenNodes;

    void drawNodeRecursive(int nIndex, Node* pNode, const std::string& strId = "");
};
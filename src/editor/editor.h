#pragma once


class Node;


class Editor
{
public:
    inline static Node* getSelectedNode() { return m_pSelectedNode; }
    inline static void setSelectedNode(Node* pNode) { m_pSelectedNode = pNode; }

private:
    static Node* m_pSelectedNode;
};

#ifndef EDITOR_H
#define EDITOR_H

Node* Editor::m_pSelectedNode = nullptr;

#endif // EDITOR_H
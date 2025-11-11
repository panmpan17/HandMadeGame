#pragma once


class Node;


class Editor
{
public:
    inline static Node* getSelectedNode() { return m_pSelectedNode; }
    inline static void setSelectedNode(Node* pNode) { m_pSelectedNode = pNode; }

private:
    static inline Node* m_pSelectedNode = nullptr;
};

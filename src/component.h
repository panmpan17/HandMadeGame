#pragma once


class Node;


class Component
{
public:
    virtual ~Component() = default;

    virtual bool isIDrawable() = 0;

    virtual bool isIUpdatable() = 0;

    virtual void setNode(Node* pNode) { m_pNode = pNode; }
    virtual Node* getNode() const { return m_pNode; }

protected:
    Node* m_pNode = nullptr;
};
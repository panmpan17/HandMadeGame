#pragma once


class Node;


class Component
{
public:
    virtual ~Component() = default;

    virtual bool isIDrawable() const = 0;

    virtual bool isUpdatable() const = 0;

    virtual void setNode(Node* pNode) { m_pNode = pNode; }
    virtual Node* getNode() const { return m_pNode; }

    virtual void draw() = 0;
    virtual void update(float deltaTime) = 0;

protected:
    Node* m_pNode = nullptr;
};

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


class IUpdatable : public Component
{
public:
    virtual ~IUpdatable() = default;

    virtual bool isIDrawable() override { return false; }
    virtual bool isIUpdatable() override { return true; }

    virtual void update(float deltaTime) = 0;
};

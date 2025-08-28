#pragma once

#include "../iserializable.h"


class Node;


class Component : public ISerializable
{
public:
    virtual ~Component() = default;

    virtual bool isIDrawable() const = 0;

    virtual bool isUpdatable() const = 0;

    virtual void setNode(Node* pNode) { m_pNode = pNode; }
    virtual Node* getNode() const { return m_pNode; }

    virtual void draw() = 0;
    virtual void update(float deltaTime) = 0;

    void serializedTo(DataSerializer& serializer) const override;
    virtual void deserializeField(const std::string_view& strFieldName, const std::string_view& strFieldValue) override;

protected:
    Node* m_pNode = nullptr;

    virtual void serializeToWrapper(DataSerializer& serializer) const {};
};

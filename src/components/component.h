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

    virtual std::string getTypeName() const { return "Component"; };

    void serializedTo(DataSerializer& serializer) const override;
    virtual void deserializeField(const std::string_view& strFieldName, const std::string_view& strFieldValue) override;
    virtual void onNodeFinishedDeserialization() {}

protected:
    Node* m_pNode = nullptr;

    virtual void serializeToWrapper(DataSerializer& serializer) const {};
};


#define COMPONENT_REGISTER_SERIALIZABLE(T)public:\
    inline std::string getTypeName() const override { return #T; }\
    virtual void deserializeField(const std::string_view& strFieldName, const std::string_view& strFieldValue) override;\
    virtual void onNodeFinishedDeserialization() override;\
protected:\
    virtual void serializeToWrapper(DataSerializer& serializer) const override;


#define COMPONENT_REGISTER_SERIALIZABLE_(T)public:\
    inline std::string getTypeName() const override { return #T; }\
    virtual void deserializeField(const std::string_view& strFieldName, const std::string_view& strFieldValue) override;\
    virtual void onNodeFinishedDeserialization() override;\
protected:\
    virtual void serializeToWrapper(DataSerializer& serializer) const override;

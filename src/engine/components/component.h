#pragma once

#include <typeinfo>
#include "../core/serialization/iserializable.h"
#include "../core/serialization/type_registry.h"


class Node;


class Component : public ISerializable
{
public:
    virtual ~Component() = default;

    virtual void onAddToNode() {}

    virtual bool isIDrawable() const = 0;

    virtual bool isUpdatable() const = 0;

    virtual void setNode(Node* pNode) { m_pNode = pNode; }
    virtual Node* getNode() const { return m_pNode; }

    virtual void onStart() {}

    virtual void draw() = 0;
    virtual void update(float deltaTime) = 0;

    virtual std::string getTypeName() const { return typeid(*this).name(); };

    void serializedTo(DataSerializer& serializer) const override;
    virtual bool deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue) override;
    virtual void onNodeFinishedDeserialization() {}

protected:
    Node* m_pNode = nullptr;

    virtual void serializeToWrapper(DataSerializer& serializer) const {};
};


#define COMPONENT_REGISTER_SERIALIZABLE(T)public:\
    inline std::string getTypeName() const override { return #T; }\
    virtual bool deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue) override;\
    virtual void onNodeFinishedDeserialization() override;\
protected:\
    virtual void serializeToWrapper(DataSerializer& serializer) const override;


#define COMPONENT_REGISTER_SERIALIZABLE_(T)public:\
    inline std::string getTypeName() const override { return #T; }\
    virtual bool deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue) override;\
    virtual void onNodeFinishedDeserialization() override;\
protected:\
    virtual void serializeToWrapper(DataSerializer& serializer) const override;

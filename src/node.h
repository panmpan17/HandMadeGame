#pragma once

#include <linmath.h>
#include "expandable_array.h"
#include "components/component.h"


class Component;
class DataSerializer;
class DataDeserializer;

class Node : public ISerializable
{
public:
    Node() {}
    Node(float fX, float fY, float fZ, float fRotationZ);
    ~Node();

    inline void setPosition(float fX, float fY) {
        m_position[0] = fX;
        m_position[1] = fY;
    }
    inline void setPosition(float fX, float fY, float fZ) {
        m_position[0] = fX;
        m_position[1] = fY;
        m_position[2] = fZ;
    }
    inline void setPosition(const vec3& position) { m_position[0] = position[0]; m_position[1] = position[1]; m_position[2] = position[2]; }

    inline void move(float fX, float fY) {
        m_position[0] += fX;
        m_position[1] += fY;
    }
    inline void move(float fX, float fY, float fZ) {
        m_position[0] += fX;
        m_position[1] += fY;
        m_position[2] += fZ;
    }

    inline const vec3& getPosition() const { return const_cast<vec3&>(m_position); }
    inline float getPositionX() const { return m_position[0]; }
    inline float getPositionY() const { return m_position[1]; }
    inline float getPositionZ() const { return m_position[2]; }

    inline void setRotation(float fRotation) { m_fRotation = fRotation; }
    inline float getRotation() const { return m_fRotation; }

    // inline void setComponent(Component* pComponent) { m_pComponent = pComponent; m_pComponent->setNode(this); }
    void addComponent(Component* pComponent);
    // inline Component* getComponent() const { return m_pComponent; }

    void update(float deltaTime);
    void draw();

    void setActive(bool isActive) { m_bIsActive = isActive; }
    bool isActive() const { return m_bIsActive; }

    void serializedTo(DataSerializer& serializer) const override;
    bool deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue) override;
    void onFinishedDeserialization();

    friend std::ostream& operator<<(std::ostream& os, const Node& node)
    {
        os << "Node(pos=" <<  node.m_position[0] << "," <<  node.m_position[1] << "," <<  node.m_position[2] << "; "
           << "rotation=" << node.m_fRotation << "; "
           << "active=" << node.m_bIsActive
           << " )";
        return os;
    }

private:
    vec3 m_position = {0.f, 0.f, 0.f};
    float m_fRotation = 0;
    bool m_bIsActive = true;

    PointerExpandableArray<Component*> m_oComponentArray = PointerExpandableArray<Component*>(5);
};
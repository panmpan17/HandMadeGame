#pragma once

#include <linmath.h>
#include "expandable_array.h"
#include "components/component.h"
#include "vector.h"
#include "quaternion.h"


class Component;
class DataSerializer;
class DataDeserializer;

class Node : public ISerializable
{
public:
    Node() {}
    Node(float fX, float fY, float fZ, float fRotationZ);
    ~Node();

    inline void setPosition(float fX, float fY)
    {
        m_vecPosition.x = fX;
        m_vecPosition.y = fY;
    }
    inline void setPosition(float fX, float fY, float fZ)
    {
        m_vecPosition.x = fX;
        m_vecPosition.y = fY;
        m_vecPosition.z = fZ;
    }
    inline void setPosition(const vec3& position) { m_vecPosition.x = position[0]; m_vecPosition.y = position[1]; m_vecPosition.z = position[2]; }
    inline void setPosition(const Vector2& position) { m_vecPosition.x = position.x; m_vecPosition.y = position.y; }
    inline void setPosition(const Vector3& position) { m_vecPosition.x = position.x; m_vecPosition.y = position.y; m_vecPosition.z = position.z; }

    inline void move(float fX, float fY)
    {
        m_vecPosition.x += fX;
        m_vecPosition.y += fY;
    }
    inline void move(float fX, float fY, float fZ)
    {
        m_vecPosition.x += fX;
        m_vecPosition.y += fY;
        m_vecPosition.z += fZ;
    }
    inline void move(const Vector2& position) { m_vecPosition += position; }
    inline void move(const Vector3& position) { m_vecPosition += position; }

    inline const Vector3& getPosition() const { return m_vecPosition; }
    inline float getPositionX() const { return m_vecPosition.x; }
    inline float getPositionY() const { return m_vecPosition.y; }
    inline float getPositionZ() const { return m_vecPosition.z; }

    inline void setRotation(float fRotation) { m_fRotation = fRotation; }
    inline float getRotation() const { return m_fRotation; }

    inline const Quaternion& getRotationQuaternion() const { return m_oRotationQuaternion; }
    inline void setRotationQuaternion(const Quaternion& quat) { m_oRotationQuaternion = quat; }
    inline void rotateQuaternion(const Quaternion& quat) { m_oRotationQuaternion = m_oRotationQuaternion * quat; }

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

    inline int getComponentCount() const { return m_oComponentArray.getSize(); }
    inline Component* getComponent(int nIndex) const { return m_oComponentArray.getElement(nIndex); }

    friend std::ostream& operator<<(std::ostream& os, const Node& node)
    {
        os << "Node(pos=" <<  node.m_vecPosition.x << "," <<  node.m_vecPosition.y << "," <<  node.m_vecPosition.z << "; "
           << "rotation=" << node.m_fRotation << "; "
           << "active=" << node.m_bIsActive
           << " )";
        return os;
    }

private:
    Vector3 m_vecPosition;
    float m_fRotation = 0;
    Quaternion m_oRotationQuaternion = Quaternion(1, 0, 0, 0);
    bool m_bIsActive = true;

    PointerExpandableArray<Component*> m_oComponentArray = PointerExpandableArray<Component*>(5);
};
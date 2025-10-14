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
        m_bWorldMatrixDirty = true;
    }
    inline void setPosition(float fX, float fY, float fZ)
    {
        m_vecPosition.x = fX;
        m_vecPosition.y = fY;
        m_vecPosition.z = fZ;
        m_bWorldMatrixDirty = true;
    }
    inline void setPosition(const Vector2& position) { m_vecPosition.x = position.x; m_vecPosition.y = position.y; m_bWorldMatrixDirty = true; }
    inline void setPosition(const Vector3& position) { m_vecPosition.x = position.x; m_vecPosition.y = position.y; m_vecPosition.z = position.z; m_bWorldMatrixDirty = true; }

    inline void move(float fX, float fY)
    {
        m_vecPosition.x += fX;
        m_vecPosition.y += fY;
        m_bWorldMatrixDirty = true;
    }
    inline void move(float fX, float fY, float fZ)
    {
        m_vecPosition.x += fX;
        m_vecPosition.y += fY;
        m_vecPosition.z += fZ;
        m_bWorldMatrixDirty = true;
    }
    inline void move(const Vector2& position) { m_vecPosition += position; m_bWorldMatrixDirty = true; }
    inline void move(const Vector3& position) { m_vecPosition += position; m_bWorldMatrixDirty = true; }

    inline const Vector3& getPosition() const { return m_vecPosition; }
    inline float getPositionX() const { return m_vecPosition.x; }
    inline float getPositionY() const { return m_vecPosition.y; }
    inline float getPositionZ() const { return m_vecPosition.z; }

    inline void setRotation(float fRotation) { m_fRotation = fRotation; }
    inline float getRotation() const { return m_fRotation; }

    inline const Quaternion& getRotationQuaternion() const { return m_oRotationQuaternion; }
    inline void setRotationQuaternion(const Quaternion& quat) { m_oRotationQuaternion = quat; m_bWorldMatrixDirty = true; }
    inline void rotateQuaternion(const Quaternion& quat) { m_oRotationQuaternion = m_oRotationQuaternion * quat; m_bWorldMatrixDirty = true; }

    // inline void setComponent(Component* pComponent) { m_pComponent = pComponent; m_pComponent->setNode(this); }
    void addComponent(Component* pComponent);
    // inline Component* getComponent() const { return m_pComponent; }

    void onStart();

    void update(float deltaTime);
    void draw();

    inline void setActive(bool isActive) { m_bIsActive = isActive; }
    inline bool isActive() const { return m_bIsActive; }

    void serializedTo(DataSerializer& serializer) const override;
    bool deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue) override;
    void onFinishedDeserialization();

    inline int getComponentCount() const { return m_oComponentArray.getSize(); }
    inline Component* getComponent(int nIndex) const { return m_oComponentArray.getElement(nIndex); }

    inline int getChildNodeCount() const { return m_oChildNodeArray.getSize(); }
    inline Node* getChildNode(int nIndex) const { return m_oChildNodeArray.getElement(nIndex); }
    inline void addChildNode(Node* pNode) { m_oChildNodeArray.addElement(pNode); pNode->m_pParentNode = this; }
    // inline void removeChildNode(int nIndex) { m_oChildNodeArray.removeElement(nIndex); }

    inline const mat4x4& getWorldMatrix()
    {
        if (m_bWorldMatrixDirty || (m_pParentNode && m_pParentNode->isMatrixDirty()))
        {
            mat4x4_translate(m_oWorldMatrixCache, m_vecPosition.x, m_vecPosition.y, m_vecPosition.z);
            mat4x4 rotationMatrix;
            m_oRotationQuaternion.toMat4x4(rotationMatrix);
            mat4x4_mul(m_oWorldMatrixCache, m_oWorldMatrixCache, rotationMatrix);

            if (m_pParentNode)
            {
                mat4x4_mul(m_oWorldMatrixCache, m_pParentNode->getWorldMatrix(), m_oWorldMatrixCache);
            }

            m_bWorldMatrixDirty = false;
        }
        return m_oWorldMatrixCache;
    }
    inline bool isMatrixDirty() const { return m_bWorldMatrixDirty; }

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

    mat4x4 m_oWorldMatrixCache;
    bool m_bWorldMatrixDirty = true;

    PointerExpandableArray<Component*> m_oComponentArray = PointerExpandableArray<Component*>(5);

    Node* m_pParentNode = nullptr;
    PointerExpandableArray<Node*> m_oChildNodeArray = PointerExpandableArray<Node*>(4);
};
#pragma once

#include <linmath.h>
#include "expandable_array.h"
#include "components/component.h"
#include "vector.h"
#include "quaternion.h"
#include "input_handle.h"


class Component;
class DataSerializer;
class DataDeserializer;

class Node : public ISerializable
{
public:
    Node() { m_vecPosition.x = 0; m_vecPosition.y = 0; m_vecPosition.z = 0; }
    Node(float fX, float fY, float fZ);
    ~Node();

    inline void setPosition(float fX, float fY)
    {
        m_vecPosition.x = fX;
        m_vecPosition.y = fY;
        m_bWorldMatrixDirty = true;
        m_bChildMatrixDirty = true;
        m_onPositionChanged.invoke();
    }
    inline void setPosition(float fX, float fY, float fZ)
    {
        m_vecPosition.x = fX;
        m_vecPosition.y = fY;
        m_vecPosition.z = fZ;
        m_bWorldMatrixDirty = true;
        m_bChildMatrixDirty = true;
        m_onPositionChanged.invoke();
    }
    inline void setPosition(const Vector2& position) { m_vecPosition.x = position.x; m_vecPosition.y = position.y; m_bWorldMatrixDirty = true; m_onPositionChanged.invoke(); }
    inline void setPosition(const Vector3& position) { m_vecPosition.x = position.x; m_vecPosition.y = position.y; m_vecPosition.z = position.z; m_bWorldMatrixDirty = true; m_onPositionChanged.invoke(); }

    inline void move(float fX, float fY)
    {
        m_vecPosition.x += fX;
        m_vecPosition.y += fY;
        m_bWorldMatrixDirty = true;
        m_bChildMatrixDirty = true;
        m_onPositionChanged.invoke();
    }
    inline void move(float fX, float fY, float fZ)
    {
        m_vecPosition.x += fX;
        m_vecPosition.y += fY;
        m_vecPosition.z += fZ;
        m_bWorldMatrixDirty = true;
        m_bChildMatrixDirty = true;
        m_onPositionChanged.invoke();
    }
    inline void move(const Vector2& position) { m_vecPosition += position; m_bWorldMatrixDirty = true; m_bChildMatrixDirty = true; m_onPositionChanged.invoke(); }
    inline void move(const Vector3& position) { m_vecPosition += position; m_bWorldMatrixDirty = true; m_bChildMatrixDirty = true; m_onPositionChanged.invoke(); }

    inline const Vector3& getPosition() const { return m_vecPosition; }
    inline float getPositionX() const { return m_vecPosition.x; }
    inline float getPositionY() const { return m_vecPosition.y; }
    inline float getPositionZ() const { return m_vecPosition.z; }

    inline const Quaternion& getRotationQuaternion() const { return m_oRotationQuaternion; }
    inline void setRotationQuaternion(const Quaternion& quat) { m_oRotationQuaternion = quat; m_bWorldMatrixDirty = true; m_bChildMatrixDirty = true; m_onRotationChanged.invoke(); }
    inline void rotateQuaternion(const Quaternion& quat) { m_oRotationQuaternion = m_oRotationQuaternion * quat; m_bWorldMatrixDirty = true; m_bChildMatrixDirty = true; m_onRotationChanged.invoke(); }

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
        if (m_bWorldMatrixDirty || (m_pParentNode && m_pParentNode->isChildMatrixDirty()))
        {
            if (m_pParentNode)
            {
                mat4x4_dup(m_oWorldMatrixCache, m_pParentNode->getWorldMatrix());
                mat4x4_translate_in_place(m_oWorldMatrixCache, m_vecPosition.x, m_vecPosition.y, m_vecPosition.z);
            }
            else
            {
                mat4x4_translate(m_oWorldMatrixCache, m_vecPosition.x, m_vecPosition.y, m_vecPosition.z);
            }

            mat4x4 rotationMatrix;
            m_oRotationQuaternion.toMat4x4(rotationMatrix);
            mat4x4_mul(m_oWorldMatrixCache, m_oWorldMatrixCache, rotationMatrix);

            m_bWorldMatrixDirty = false;
        }
        return m_oWorldMatrixCache;
    }
    inline void setChildMatrix(const mat4x4& matrix)
    {
        mat4x4_dup(m_oWorldMatrixCache, matrix);
        m_bWorldMatrixDirty = false;
        m_bChildMatrixDirty = false;
    }
    inline bool isChildMatrixDirty() const { return m_bChildMatrixDirty; }

    friend std::ostream& operator<<(std::ostream& os, const Node& node)
    {
        os << "Node(pos=" <<  node.m_vecPosition.x << "," <<  node.m_vecPosition.y << "," <<  node.m_vecPosition.z << "; "
           << "active=" << node.m_bIsActive
           << " )";
        return os;
    }

    void registerOnPositionChangedListener(const std::function<void()>& listener) { m_onPositionChanged.add(listener); }
    void registerOnRotationChangedListener(const std::function<void()>& listener) { m_onRotationChanged.add(listener); }

private:
    Vector3 m_vecPosition;
    VoidEvent m_onPositionChanged;

    Quaternion m_oRotationQuaternion = Quaternion(1, 0, 0, 0);
    VoidEvent m_onRotationChanged;

    bool m_bIsActive = true;

    mat4x4 m_oWorldMatrixCache;
    bool m_bWorldMatrixDirty = true;
    bool m_bChildMatrixDirty = true;

    PointerExpandableArray<Component*> m_oComponentArray = PointerExpandableArray<Component*>(5);

    Node* m_pParentNode = nullptr;
    PointerExpandableArray<Node*> m_oChildNodeArray = PointerExpandableArray<Node*>(4);
};
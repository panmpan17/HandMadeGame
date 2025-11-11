#include "node.h"
#include <iostream>
#include "../debug_macro.h"
#include "../serialization/serializer.h"
#include "../math/random.h"
#include "../../components/component.h"
#include "../../components/drawable_interface.h"


Node::Node(float fX, float fY, float fZ)
{
    m_vecPosition.x = fX;
    m_vecPosition.y = fY;
    m_vecPosition.z = fZ;

    m_nID = generateRandomUUID();
}

Node::~Node()
{
    // NOTE: m_oComponentArray should auto destruct and call the destructors of all components.
}

void Node::serializedTo(DataSerializer& serializer) const
{
    serializer.startClassHeader("Node");
    serializer.ADD_ATTRIBUTES(m_nID);
    serializer.ADD_ATTRIBUTES(m_vecPosition);
    serializer.ADD_ATTRIBUTES(m_bIsActive);
    serializer.endClassHeader();

    for (int i = 0; i < m_oComponentArray.getSize(); ++i)
    {
        Component* pComponent = m_oComponentArray.getElement(i);
        if (pComponent)
        {
            serializer << pComponent;
        }
    }
}

bool Node::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    DESERIALIZE_FIELD(m_nID);
    DESERIALIZE_FIELD(m_vecPosition);
    DESERIALIZE_FIELD(m_bIsActive);

    return false;
}

void Node::onFinishedDeserialization()
{
    int nSize = m_oComponentArray.getSize();
    for (int i = 0; i < nSize; ++i)
    {
        Component* pComponent = m_oComponentArray.getElement(i);
        if (pComponent)
        {
            pComponent->onNodeFinishedDeserialization();
        }
    }
}

void Node::onStart()
{
    int nSize = m_oComponentArray.getSize();
    for (int i = 0; i < nSize; ++i)
    {
        Component* pComponent = m_oComponentArray.getElement(i);
        if (pComponent)
        {
            // try
            // {
            pComponent->onStart();
            // }
            // catch (const std::runtime_error& e) {
            //     LOGERR("Runtime error in component onStart: {}", e.what());
            // }
            // catch (const std::exception& e) {
            //     LOGERR("Standard exception in component onStart: {}", e.what());
            // }
            // catch (...) {
            //     LOGERR("Unknown exception in component onStart");
            // }
        }
    }

    for (int i = 0; i < m_oChildNodeArray.getSize(); ++i)
    {
        Node* pChildNode = m_oChildNodeArray.getElement(i);
        if (pChildNode)
        {
            pChildNode->onStart();
        }
    }
}

void Node::update(float deltaTime)
{
    // Update logic for the node, if any
    int nSize = m_oComponentArray.getSize();
    for (int i = 0; i < nSize; ++i)
    {
        Component* pComponent = m_oComponentArray.getElement(i);
        if (pComponent && pComponent->isUpdatable())
        {
            try
            {
                pComponent->update(deltaTime);
            }
            catch (const std::runtime_error& e) {
                LOGERR("Runtime error in component update: {}", e.what());
            }
            catch (const std::exception& e) {
                LOGERR("Standard exception in component update: {}", e.what());
            }
            catch (...) {
                LOGERR("Unknown exception in component update");
            }
        }
    }

    for (int i = 0; i < m_oChildNodeArray.getSize(); ++i)
    {
        Node* pChildNode = m_oChildNodeArray.getElement(i);
        if (pChildNode && pChildNode->isActive())
        {
            pChildNode->update(deltaTime);
        }
    }
}

void Node::draw()
{
    // Draw logic for the node, if any
    int nSize = m_oComponentArray.getSize();
    for (int i = 0; i < nSize; ++i)
    {
        Component* pComponent = m_oComponentArray.getElement(i);
        if (pComponent && pComponent->isIDrawable())
        {
            try
            {
                static_cast<IDrawable*>(pComponent)->draw();
            }
            catch (const std::runtime_error& e) {
                LOGERR("Runtime error in component draw: {}", e.what());
            }
            catch (const std::exception& e) {
                LOGERR("Standard exception in component draw: {}", e.what());
            }
            catch (...) {
                LOGERR("Unknown exception in component draw");
            }
        }
    }

    for (int i = 0; i < m_oChildNodeArray.getSize(); ++i)
    {
        Node* pChildNode = m_oChildNodeArray.getElement(i);
        if (pChildNode && pChildNode->isActive())
        {
            pChildNode->draw();
        }
    }

    m_bChildMatrixDirty = false;
}

void Node::drawDepth()
{
    // Draw logic for the node, if any
    int nSize = m_oComponentArray.getSize();
    for (int i = 0; i < nSize; ++i)
    {
        Component* pComponent = m_oComponentArray.getElement(i);
        if (pComponent && pComponent->isIDrawable())
        {
            try
            {
                static_cast<IDrawable*>(pComponent)->drawDepth();
            }
            catch (const std::runtime_error& e) {
                LOGERR("Runtime error in component draw: {}", e.what());
            }
            catch (const std::exception& e) {
                LOGERR("Standard exception in component draw: {}", e.what());
            }
            catch (...) {
                LOGERR("Unknown exception in component draw");
            }
        }
    }

    for (int i = 0; i < m_oChildNodeArray.getSize(); ++i)
    {
        Node* pChildNode = m_oChildNodeArray.getElement(i);
        if (pChildNode && pChildNode->isActive())
        {
            pChildNode->drawDepth();
        }
    }

    m_bChildMatrixDirty = false;
}

void Node::addComponent(Component* pComponent)
{
    if (pComponent == nullptr) return;
    m_oComponentArray.addElement(pComponent);
    pComponent->setNode(this);
    pComponent->onAddToNode();
}
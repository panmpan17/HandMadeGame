#include "node.h"
#include "components/component.h"
#include "draw/drawable_interface.h"
#include <iostream>
#include "debug_macro.h"
#include "serializer.h"
#include "random.h"


Node::Node(float fX, float fY, float fZ, float fRotationZ) : m_fRotation(fRotationZ)
{
    m_position[0] = fX;
    m_position[1] = fY;
    m_position[2] = fZ;

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
    serializer.ADD_ATTRIBUTES(m_position);
    serializer.ADD_ATTRIBUTES(m_fRotation);
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
    DESERIALIZE_FIELD(m_position);
    DESERIALIZE_FIELD(m_fRotation);
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
                LOGERRLN_EX("Runtime error in component update: {}", e.what());
            }
            catch (const std::exception& e) {
                LOGERRLN_EX("Standard exception in component update: {}", e.what());
            }
            catch (...) {
                LOGERRLN("Unknown exception in component update");
            }
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
                LOGERRLN_EX("Runtime error in component draw: {}", e.what());
            }
            catch (const std::exception& e) {
                LOGERRLN_EX("Standard exception in component draw: {}", e.what());
            }
            catch (...) {
                LOGERRLN("Unknown exception in component draw");
            }
        }
    }
}

void Node::addComponent(Component* pComponent)
{
    if (pComponent == nullptr) return;
    m_oComponentArray.addElement(pComponent);
    pComponent->setNode(this);
}
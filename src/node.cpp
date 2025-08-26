#include "node.h"
#include "components/component.h"
#include "draw/drawable_interface.h"
#include <iostream>
#include "debug_macro.h"


Node::~Node()
{
    // NOTE: m_oComponentArray should auto destruct and call the destructors of all components.
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
#include "node.h"
#include "components/component.h"
#include "draw/drawable_interface.h"
#include <iostream>
#include "debug_macro.h"


Node::~Node()
{
    for (int i = 0; i < m_nComponentCount; ++i)
    {
        if (m_pComponents[i])
        {
            delete m_pComponents[i];
        }
    }
}


void Node::update(float deltaTime)
{
    // Update logic for the node, if any
    for (int i = 0; i < m_nComponentCount; ++i)
    {
        if (m_pComponents[i] && m_pComponents[i]->isUpdatable())
        {
            try
            {
                m_pComponents[i]->update(deltaTime);
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
    for (int i = 0; i < m_nComponentCount; ++i)
    {
        if (m_pComponents[i] && m_pComponents[i]->isIDrawable())
        {
            try
            {
                static_cast<IDrawable*>(m_pComponents[i])->draw();
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

    // Find an empty slot in the component array
    for (int i = 0; i < m_nComponentCount; ++i)
    {
        if (m_pComponents[i] == nullptr)
        {
            m_pComponents[i] = pComponent;
            pComponent->setNode(this);
            return;
        }
    }

    // TODO: Handle case where no empty slot is found, e.g., resize the array or log an error
}
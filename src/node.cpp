#include "node.h"
#include "components/component.h"
#include "draw/drawable_interface.h"


void Node::update(float deltaTime)
{
    // Update logic for the node, if any
    for (int i = 0; i < m_nComponentCount; ++i)
    {
        if (m_pComponents[i] && m_pComponents[i]->isUpdatable())
        {
            m_pComponents[i]->update(deltaTime);
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
            static_cast<IDrawable*>(m_pComponents[i])->draw();
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
#include "node.h"
#include "component.h"
#include "draw/drawable_interface.h"


void Node::update(float deltaTime)
{
    // Update logic for the node, if any
    if (m_pComponent && m_pComponent->isIUpdatable()) {
        // m_pComponent->update(deltaTime);
    }
}

void Node::draw()
{
    // Draw logic for the node, if any
    if (m_pComponent && m_pComponent->isIDrawable())
    {
        static_cast<IDrawable*>(m_pComponent)->draw();
    }
}
#include "hierarchy_view.h"

#include <sstream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../world.h"
#include "../node.h"
#include "../components/component.h"
#include "../debug_macro.h"
#include "../serialization/type_registry.h"


constexpr int INDENT_SIZE = 10;


HierarchyView::HierarchyView()
{
}

HierarchyView::~HierarchyView()
{
}

void HierarchyView::update(float fDeltaTime)
{
    if (ImGui::Begin("Hierarchy View", &m_bCollapsed))
    {
        ImGui::SetWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
        ImGui::SetWindowPos(ImVec2(5, 155), ImGuiCond_FirstUseEver);

        const TypeRegistry& typeRegistry = TypeRegistry::instance();

        if (WorldScene::current)
        {
            int nNodeCount = WorldScene::current->getNodeCount();
            for (int i = 0; i < nNodeCount; ++i)
            {
                Node* pNode = WorldScene::current->getNode(i);
                if (pNode)
                {
                    drawNodeRecursive(i, pNode);
                }
            }
        }
        else
        {
            ImGui::Text("No World Scene");
        }
    }

    ImGui::End();
}

void HierarchyView::drawNodeRecursive(int nIndex, Node* pNode)
{
    ImGui::Text("Node %d", nIndex);

    ImGui::Indent(INDENT_SIZE);
    int nComponentCount = pNode->getComponentCount();
    for (int j = 0; j < nComponentCount; ++j)
    {
        Component* pComponent = pNode->getComponent(j);
        if (pComponent)
        {
            ImGui::Text("[%s]", TypeRegistry::instance().getTitlizedName(typeid(*pComponent)).c_str());
        }
    }

    int nChildCount = pNode->getChildNodeCount();
    for (int k = 0; k < nChildCount; ++k)
    {
        Node* pChildNode = pNode->getChildNode(k);
        if (pChildNode)
        {
            drawNodeRecursive(k, pChildNode);
        }
    }
    ImGui::Unindent(INDENT_SIZE);
}
#include "hierarchy_view.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../world.h"
#include "../node.h"
#include "../components/component.h"

HierarchyView::HierarchyView()
{
}

HierarchyView::~HierarchyView()
{
}

void HierarchyView::update(float fDeltaTime)
{
    ImGui::Begin("Hierarchy View", &m_bCollapsed);
    ImGui::SetWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::SetWindowPos(ImVec2(5, 135), ImGuiCond_FirstUseEver);

    if (WorldScene::current)
    {
        int nNodeCount = WorldScene::current->getNodeCount();
        for (int i = 0; i < nNodeCount; ++i)
        {
            Node* pNode = WorldScene::current->getNode(i);
            if (pNode)
            {
                ImGui::Text("Node %d", i);

                int nComponentCount = pNode->getComponentCount();
                for (int j = 0; j < nComponentCount; ++j)
                {
                    Component* pComponent = pNode->getComponent(j);
                    if (pComponent)
                    {
                        ImGui::Text("  %s", pComponent->getTypeName().c_str());
                    }
                }
            }
        }
    }
    else
    {
        ImGui::Text("No World Scene");
    }

    ImGui::End();
}
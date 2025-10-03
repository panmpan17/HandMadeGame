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


HierarchyView::HierarchyView()
{
    const TypeRegistry& typeRegistry = TypeRegistry::instance();

    int nNodeCount = WorldScene::current->getNodeCount();
    for (int i = 0; i < nNodeCount; ++i)
    {
        Node* pNode = WorldScene::current->getNode(i);
        if (pNode)
        {
            int nComponentCount = pNode->getComponentCount();
            for (int j = 0; j < nComponentCount; ++j)
            {
                Component* pComponent = pNode->getComponent(j);

                if (pComponent)
                {
                    LOGLN_EX("Component type: {}, {}", typeid(*pComponent).name(), typeRegistry.getTitlizedName(typeid(*pComponent)));
                }
            }
        }
    }
}

HierarchyView::~HierarchyView()
{
}

void HierarchyView::update(float fDeltaTime)
{
    ImGui::Begin("Hierarchy View", &m_bCollapsed);
    ImGui::SetWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::SetWindowPos(ImVec2(5, 135), ImGuiCond_FirstUseEver);

    const TypeRegistry& typeRegistry = TypeRegistry::instance();

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
                        ImGui::Text("  %s", typeRegistry.getTitlizedName(typeid(*pComponent)).c_str());
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
#include "hierarchy_view.h"

#include <sstream>
#include <imgui_internal.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "editor.h"
#include "../engine/components/component.h"
#include "../engine/core/scene/world.h"
#include "../engine/core/scene/node.h"
#include "../engine/core/debug_macro.h"
#include "../engine/core/serialization/type_registry.h"


constexpr int INDENT_SIZE = 10;

const ImVec4 TRANSPARENT_BUTTON = ImVec4(0, 0, 0, 0);
const ImVec4 TRANSPARENT_BUTTON_HOVERED = ImVec4(0.3f, 0.3f, 0.3f, 0.3f);
const ImVec4 TRANSPARENT_BUTTON_ACTIVE = ImVec4(0.4f, 0.4f, 0.4f, 0.4f);

const ImVec4 SELECTED_BUTTON = ImVec4(0.1f, 0.5f, 0.81f, 0.6f);
const ImVec4 SELECTED_BUTTON_HOVERED = ImVec4(0.1f, 0.5f, 0.81f, 0.8f);
const ImVec4 SELECTED_BUTTON_ACTIVE = ImVec4(0.1f, 0.5f, 0.81f, 1.0f);


HierarchyView::HierarchyView()
{
}

HierarchyView::~HierarchyView()
{
}

void HierarchyView::update()
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
                    drawNodeRecursive(i, pNode, std::to_string(i));
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

bool BeginButtonDropDown(const std::string& strLabel, bool pointToRight = true)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    float x = window->Pos.x + window->DC.CursorPos.x - window->Pos.x; // ImGui::GetCursorPosX() -  window->Scroll.x;
    float y = window->Pos.y + window->DC.CursorPos.y - window->Pos.y; // ImGui::GetCursorPosY() - window->Scroll.y;

    constexpr float BUTTON_SIZE = 16.f;
    constexpr float ARROW_SIZE = 5.f;

    ImVec2 size(BUTTON_SIZE, BUTTON_SIZE);
    bool pressed = ImGui::Button(("##" + strLabel).c_str(), size);

    if (pointToRight)
    {
        ImVec2 center(x + (BUTTON_SIZE / 2) - (ARROW_SIZE * 0.25f), y + (size.y / 2));

        ImVec2 a(center.x + ARROW_SIZE, center.y);
        ImVec2 b(center.x + (-0.5f * ARROW_SIZE), center.y + (-0.866f * ARROW_SIZE));
        ImVec2 c(center.x + (-0.5f * ARROW_SIZE), center.y + (0.866f * ARROW_SIZE));

        window->DrawList->AddTriangleFilled(a, b, c, ImGui::GetColorU32(ImGuiCol_Text));
    }
    else
    {
        ImVec2 center(x + (BUTTON_SIZE / 2), y + (size.y / 2) - (ARROW_SIZE * 0.25f));

        ImVec2 a(center.x + 0, center.y + ARROW_SIZE);
        ImVec2 b(center.x + (-0.866f * ARROW_SIZE), center.y + (-0.5f * ARROW_SIZE));
        ImVec2 c(center.x + (0.866f * ARROW_SIZE), center.y + (-0.5f * ARROW_SIZE));

        window->DrawList->AddTriangleFilled(a, b, c, ImGui::GetColorU32(ImGuiCol_Text));
    }

    return pressed;
}

void HierarchyView::drawNodeRecursive(int nIndex, Node* pNode, const std::string& strId/* = ""*/)
{
    int nChildCount = pNode->getChildNodeCount();

    bool bIsHidden = true;

    ImGui::PushStyleColor(ImGuiCol_Button, TRANSPARENT_BUTTON);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, TRANSPARENT_BUTTON_HOVERED);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, TRANSPARENT_BUTTON_ACTIVE);

    if (nChildCount > 0)
    {
        bIsHidden = (std::find(m_vecHideChildrenNodes.begin(), m_vecHideChildrenNodes.end(), pNode) != m_vecHideChildrenNodes.end());
        if (BeginButtonDropDown(std::format("##{}", strId), bIsHidden))
        {
            if (bIsHidden)
            {
                m_vecHideChildrenNodes.erase(std::remove(m_vecHideChildrenNodes.begin(), m_vecHideChildrenNodes.end(), pNode), m_vecHideChildrenNodes.end());
                bIsHidden = false;
            }
            else
            {
                m_vecHideChildrenNodes.push_back(pNode);
                bIsHidden = true;
            }
        }

        ImGui::SameLine();
    }

    bool bIsSelected = (Editor::getSelectedNode() == pNode);
    if (bIsSelected)
    {
        ImGui::PopStyleColor(3);
        ImGui::PushStyleColor(ImGuiCol_Button, SELECTED_BUTTON);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SELECTED_BUTTON_HOVERED);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, SELECTED_BUTTON_ACTIVE);
    }

    const std::string& strName = pNode->getName();
    std::string strDisplayName = strName.empty() ? std::format("Node {}##{}", nIndex, strId) : (strName + "##" + strId);
    if (ImGui::Button(strDisplayName.c_str()))
    {
        Editor::setSelectedNode(bIsSelected ? nullptr : pNode);
    }

    ImGui::PopStyleColor(3);

    if (!bIsHidden)
    {
        ImGui::Indent(INDENT_SIZE);

        for (int k = 0; k < nChildCount; ++k)
        {
            drawNodeRecursive(k, pNode->getChildNode(k), std::format("{}_{}", strId, k));
        }
        ImGui::Unindent(INDENT_SIZE);
    }
}

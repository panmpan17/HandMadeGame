#include "hierarchy_view.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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
    ImGui::SetWindowPos(ImVec2(0, 85), ImGuiCond_FirstUseEver);

    // ImGui::Text("Scene Hierarchy");

    ImGui::End();
}
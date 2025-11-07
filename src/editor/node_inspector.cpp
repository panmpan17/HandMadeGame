#include "node_inspector.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "editor.h"
#include "../engine/core/camera.h"
#include "../engine/core/scene/node.h"
#include "../engine/components/component.h"


NodeInspector::NodeInspector()
{
}

NodeInspector::~NodeInspector()
{
}

void NodeInspector::update(float fDeltaTime)
{
    ImGui::Begin("Node Inspector", &m_bCollapsed);
    ImGui::SetWindowSize(ImVec2(300, 130), ImGuiCond_FirstUseEver);
    ImGui::SetWindowPos(ImVec2(5, 25), ImGuiCond_FirstUseEver);

    Node* pSelectedNode = Editor::getSelectedNode();
    if (pSelectedNode)
    {
        updateTransform(pSelectedNode);

        int nComponentCount = pSelectedNode->getComponentCount();
        for (int i = 0; i < nComponentCount; ++i)
        {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            updateComponent(pSelectedNode, pSelectedNode->getComponent(i));
        }
    }
    else
    {
        ImGui::Text("No node selected.");
    }

    ImGui::End();
}

void NodeInspector::updateTransform(Node* pNode)
{
    ImGui::Text("Transform");

    Vector3 vecPosition = pNode->getPosition();
    ImGui::Text("Position");ImGui::SameLine();
    if (ImGui::InputFloat3("##NodeInspectorPosition", reinterpret_cast<float*>(&vecPosition)))
    {
        pNode->setPosition(vecPosition);
    }

    Vector3 vecEulerAngles;
    pNode->getRotationQuaternion().toEulerAngles(vecEulerAngles);
    ImGui::Text("Euler");ImGui::SameLine();
    if (ImGui::InputFloat3("##NodeInspectorRotation", reinterpret_cast<float*>(&vecEulerAngles)))
    {
        Quaternion oNewRotation = Quaternion::fromEulerAngles(vecEulerAngles);
        pNode->setRotationQuaternion(oNewRotation);
    }

    Vector3 vecScale = pNode->getScale();
    ImGui::Text("Scale");ImGui::SameLine();
    if (ImGui::InputFloat3("##NodeInspectorScale", reinterpret_cast<float*>(&vecScale)))
    {
        pNode->setScale(vecScale);
    }
}

void NodeInspector::updateComponent(Node* pNode, Component* pComponent)
{
    const std::string strTypeName = TypeRegistry::instance().getTitlizedName(typeid(*pComponent));
    ImGui::Text("%s", strTypeName.c_str());
}

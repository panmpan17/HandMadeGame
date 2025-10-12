#include "camera_inspector.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../camera.h"


CameraInspector::CameraInspector()
{
}

CameraInspector::~CameraInspector()
{
}

void CameraInspector::update(float fDeltaTime)
{
    ImGui::Begin("Camera Inspector", &m_bCollapsed);
    ImGui::SetWindowSize(ImVec2(300, 130), ImGuiCond_FirstUseEver);
    ImGui::SetWindowPos(ImVec2(5, 25), ImGuiCond_FirstUseEver);

    bool bIsOrtho = Camera::main->isUsingOrthoProjection();
    if (ImGui::Checkbox("Ortho", &bIsOrtho))
    {
        Camera::main->setUseOrthoProjection(bIsOrtho);
    }

    // vec3 vecPosition;
    // Camera::main->getPosition(vecPosition);
    // if (ImGui::InputFloat3("Position", vecPosition))
    // {
    //     Camera::main->setPosition(vecPosition[0], vecPosition[1], vecPosition[2]);
    // }

    // vec3 vecPointAt;
    // Camera::main->getPointAt(vecPointAt);
    // if (ImGui::InputFloat3("Point At", vecPointAt))
    // {
    //     Camera::main->setPointAt(vecPointAt[0], vecPointAt[1], vecPointAt[2]);
    // }

    // vec3 vecUp;
    // Camera::main->getVecUp(vecUp);
    // if (ImGui::InputFloat3("Up", vecUp))
    // {
    //     Camera::main->setVecUp(vecUp[0], vecUp[1], vecUp[2]);
    // }

    ImGui::End();
}
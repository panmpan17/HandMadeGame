#include "camera_inspector.h"

#include <linmath.h>
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
    ImGui::Begin("Camera Inspector", &m_bIsActive);
    ImGui::SetWindowSize(ImVec2(300, 80), ImGuiCond_FirstUseEver);
    ImGui::SetWindowPos(ImVec2(5, 5), ImGuiCond_FirstUseEver);

    vec3 vecPosition;
    Camera::main->getPosition(vecPosition);
    if (ImGui::InputFloat3("Position", vecPosition))
    {
        Camera::main->setPosition(vecPosition[0], vecPosition[1], vecPosition[2]);
    }

    ImGui::End();
}
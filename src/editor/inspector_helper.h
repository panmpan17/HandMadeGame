#pragma once

#include <imgui.h>

#define BOOL_FIELD(nComponentIndex, label, value) \
    ImGui::Text(label); \
    ImGui::SameLine(); \
    ImGui::Checkbox(("##Component" + std::to_string(nComponentIndex) + label).c_str(), &value);

inline bool inspectorFloatField(int nComponentIndex, const char* strLabel, float& outFloat)
{
    ImGui::Text(strLabel, "");
    ImGui::SameLine();
    return ImGui::InputFloat(("##Component" + std::to_string(nComponentIndex) + strLabel).c_str(), &outFloat);
}

#define VECTOR3_FIELD(nComponentIndex, label, vec) \
    ImGui::Text(label); \
    ImGui::SameLine(); \
    ImGui::InputFloat3(("##Component" + std::to_string(nComponentIndex) + label).c_str(), reinterpret_cast<float*>(&vec));

inline bool inspectorColorField(int nComponentIndex, const char* strLabel, Vector3& outVec)
{
    ImGui::Text(strLabel, "");
    ImGui::SameLine();
    return ImGui::ColorEdit3(("##Component" + std::to_string(nComponentIndex) + strLabel).c_str(), reinterpret_cast<float*>(&outVec));
}

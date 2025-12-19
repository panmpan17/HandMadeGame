#pragma once

#include <imgui.h>

inline bool inspectorBoolField(int nComponentIndex, const char* strLabel, bool& outValue)
{
    ImGui::Text(strLabel, "");
    ImGui::SameLine();
    return ImGui::Checkbox(("##Component" + std::to_string(nComponentIndex) + strLabel).c_str(), &outValue);
}

inline bool inspectorFloatField(int nComponentIndex, const char* strLabel, float& outFloat)
{
    ImGui::Text(strLabel, "");
    ImGui::SameLine();
    return ImGui::InputFloat(("##Component" + std::to_string(nComponentIndex) + strLabel).c_str(), &outFloat);
}

inline bool inspectorVector2Field(int nComponentIndex, const char* strLabel, Vector2& outVec)
{
    ImGui::Text(strLabel, "");
    ImGui::SameLine();
    return ImGui::InputFloat2(("##Component" + std::to_string(nComponentIndex) + strLabel).c_str(), reinterpret_cast<float*>(&outVec));
}

inline bool inspectorVector3Field(int nComponentIndex, const char* strLabel, Vector3& outVec)
{
    ImGui::Text(strLabel, "");
    ImGui::SameLine();
    return ImGui::InputFloat3(("##Component" + std::to_string(nComponentIndex) + strLabel).c_str(), reinterpret_cast<float*>(&outVec));
}

inline bool inspectorColorField(int nComponentIndex, const char* strLabel, Vector3& outVec)
{
    ImGui::Text(strLabel, "");
    ImGui::SameLine();
    return ImGui::ColorEdit3(("##Component" + std::to_string(nComponentIndex) + strLabel).c_str(), reinterpret_cast<float*>(&outVec));
}

#pragma once

#include <imgui.h>

#define BOOL_FIELD(nComponentIndex, label, value) \
    ImGui::Text(label); \
    ImGui::SameLine(); \
    ImGui::Checkbox(("##Component" + std::to_string(nComponentIndex) + label).c_str(), &value);

#define FLOAT_FIELD(nComponentIndex, label, value) \
    ImGui::Text(label); \
    ImGui::SameLine(); \
    ImGui::InputFloat(("##Component" + std::to_string(nComponentIndex) + label).c_str(), &value);

#define VECTOR3_FIELD(nComponentIndex, label, vec) \
    ImGui::Text(label); \
    ImGui::SameLine(); \
    ImGui::InputFloat3(("##Component" + std::to_string(nComponentIndex) + label).c_str(), reinterpret_cast<float*>(&vec));

#define COLOR_FIELD(nComponentIndex, label, vec) \
    ImGui::Text(label); \
    ImGui::SameLine(); \
    ImGui::ColorEdit3(("##Component" + std::to_string(nComponentIndex) + label).c_str(), reinterpret_cast<float*>(&vec));

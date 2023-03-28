/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "../ImGuiDefinitions.h"

namespace SPW {

class ImGuiDockSpace final {
public:
  ImGuiDockSpace() = default;

  static void Render(const std::string &name) {

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    ImGui::Begin(name.c_str(), nullptr, window_flags);
    ImGui::PopStyleVar(3);

    const ImGuiID dockspace_id = ImGui::GetID(name.c_str());
    ImGui::DockSpace(dockspace_id);

    ImGui::End();
  }
};

}
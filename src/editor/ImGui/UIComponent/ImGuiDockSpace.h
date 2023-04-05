/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "../ImGuiDefinitions.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"
#include <ostream>
#include <iostream>
namespace SPW {

class ImGuiDockSpace final {
public:
  ImGuiDockSpace() = default;

  static void Render(const std::string &name) {

      ImVec2 canvasSize = ImGui::GetWindowSize();
      ImVec2 windowSize = ImGui::GetWindowSize();
      std::cout << canvasSize.x << " " << canvasSize.y << std::endl;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    ImGui::Begin(name.c_str(), nullptr, window_flags);
    ImGui::PopStyleVar(3);

    if(name == "Left Dock Space"){
        const ImGuiID dock_left_id = ImGui::GetID(name.c_str());
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(canvasSize);
        ImGui::DockSpace(dock_left_id);
    }
    if(name == "Right Dock Space"){
        const ImGuiID dock_right_id = ImGui::GetID(name.c_str());
        ImGui::SetNextWindowPos(ImVec2(windowSize.x - canvasSize.x, 0));
        ImGui::SetNextWindowSize(canvasSize);
        ImGui::DockSpace(dock_right_id);
    }
    if(name == "Bottom Dock Space"){
        const ImGuiID dock_bottom_id = ImGui::GetID(name.c_str());
        ImGui::SetNextWindowPos(ImVec2(0, 400));
        ImGui::SetNextWindowSize(canvasSize);
        ImGui::DockSpace(dock_bottom_id);
    }



    ImGui::End();
  }
};

}
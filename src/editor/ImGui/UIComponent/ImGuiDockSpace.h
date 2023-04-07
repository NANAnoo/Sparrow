/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "../ImGuiDefinitions.h"
#include "ImGuiPanel.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"
#include <ostream>
#include <iostream>
namespace SPW {

class ImGuiDockSpace final {
public:
  ImGuiDockSpace() {
       docspace_Open = true;
       opt_fullscreen = true;
       opt_padding = false;
       dockspace_flags = ImGuiDockNodeFlags_None;
  }

	void Render() {

//      ImVec2 canvasSize = ImGui::GetWindowSize();
//      ImVec2 windowSize = ImGui::GetWindowSize();
//      std::cout << canvasSize.x << " " << canvasSize.y << std::endl;
//    const ImGuiViewport *viewport = ImGui::GetMainViewport();
//    ImGui::SetNextWindowViewport(viewport->ID);
//
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
//
//    constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
//    ImGui::Begin(name.c_str(), nullptr, window_flags);
//    ImGui::PopStyleVar(3);

//    if(name == "Left Dock Space"){
//        const ImGuiID dock_left_id = ImGui::GetID(name.c_str());
//        ImGui::SetNextWindowPos(ImVec2(0, 0));
//        ImGui::SetNextWindowSize(canvasSize);
//        ImGui::DockSpace(dock_left_id);
//    }
//    if(name == "Right Dock Space"){
//        const ImGuiID dock_right_id = ImGui::GetID(name.c_str());
//        ImGui::SetNextWindowPos(ImVec2(windowSize.x - canvasSize.x, 0));
//        ImGui::SetNextWindowSize(canvasSize);
//        ImGui::DockSpace(dock_right_id);
//    }
//    if(name == "Bottom Dock Space"){
//        const ImGuiID dock_bottom_id = ImGui::GetID(name.c_str());
//        ImGui::SetNextWindowPos(ImVec2(0, 400));
//        ImGui::SetNextWindowSize(canvasSize);
//        ImGui::DockSpace(dock_bottom_id);
//    }
      // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
      // because it would be confusing to have two docking targets within each others.
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
      if (opt_fullscreen)
      {
          const ImGuiViewport* viewport = ImGui::GetMainViewport();
          ImGui::SetNextWindowPos(viewport->WorkPos);
          ImGui::SetNextWindowSize(viewport->WorkSize);
          ImGui::SetNextWindowViewport(viewport->ID);
          ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
          ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
          window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
          window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
      }
      else
      {
          dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
      }

      // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
      // and handle the pass-thru hole, so we ask Begin() to not render a background.
      if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
          window_flags |= ImGuiWindowFlags_NoBackground;

      // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
      // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
      // all active windows docked into it will lose their parent and become undocked.
      // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
      // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
      if (!opt_padding)
          ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

      ImGui::Begin("DockSpace Demo", &docspace_Open, window_flags);
      if (!opt_padding)
          ImGui::PopStyleVar();

      if (opt_fullscreen)
          ImGui::PopStyleVar(2);

      // Submit the DockSpace
      ImGuiIO& io = ImGui::GetIO();
      if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
      {
          ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
          ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
      }

      if (ImGui::BeginMenuBar())
      {
          if (ImGui::BeginMenu("Options"))
          {
              // Disabling fullscreen would allow the window to be moved to the front of other windows,
              // which we can't undo at the moment without finer window depth/z control.
              ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
              ImGui::MenuItem("Padding", NULL, &opt_padding);
              ImGui::Separator();

              if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
              if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
              if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
              if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
              if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
              ImGui::Separator();

              if (ImGui::MenuItem("Close", NULL, false, docspace_Open != NULL))
                  docspace_Open = false;
              ImGui::EndMenu();
          }


          ImGui::EndMenuBar();
      }
    for(auto const& p:m_Panels){
        p->Render();
    }
      ImGui::End();


  }

  void AddPanel(std::shared_ptr<SPW::ImGuiPanel> panel){
      m_Panels.push_back(panel);
  }

  std::vector<std::shared_ptr<SPW::ImGuiPanel>> GetPanels()
  {
	  	return m_Panels;
  }

private:
    std::vector<std::shared_ptr<SPW::ImGuiPanel>> m_Panels;
    bool docspace_Open ;
    bool opt_fullscreen;
    bool opt_padding;
    ImGuiDockNodeFlags dockspace_flags;
};

}
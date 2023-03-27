/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGuiDefinitions.h"
#include "UIComponent/ImGuiDockSpace.h"
#include "UIComponent/ImGuiPanel.h"
#include "UIComponent/ImGuiMenuBar.h"
#include "UIComponent/ImGuiObjectPanel.h"
#include "UIComponent/ImGuiInspectorPanel.h"
#include "UIComponent/ImGuiTreeNodePanel.h"

namespace SPW
{

class ImGuiManager
{
public:
  ImGuiManager() = default;

  void Init(GLFWwindow* window);
  void Begin();
  void End();
  void CleanUp();
  void ShowDemoWindow(bool show_demo_window);
  void EnableViewport();

  GLFWwindow* GetWindowHandle() const {return windowHandle;}

  std::shared_ptr<ImGuiObjectPanel>     GetProfilingPanel() { return m_ObjectPanel; }
  std::shared_ptr<ImGuiInspectorPanel>  GetInspectorPanel() { return m_InspectorPanel; }

  template <UIComponentType C>
  void RenderUIComponent() {}

  template <UIComponentType C>
  void RenderUIComponent(const std::string& name) {  }

  template <UIComponentType C>
  void RenderUIComponent(std::string&& name) { }

  template<>
  void RenderUIComponent<UIComponentType::Panel>(std::string&& name)
  {
    // Show a simple window that we create ourselves (use a Begin/End pair to created a named window)
    ImGui::Begin(name.c_str());
    ImGui::Checkbox("Show Demo Window", &show_demo_window);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    if (show_demo_window)
      ShowDemoWindow(&show_demo_window);
  }

  template<>
  void RenderUIComponent<UIComponentType::Dockspace>(std::string&& name)
  {
    ImGuiDockSpace::Render(std::forward<std::string>(name));
  }

  template<>
  void RenderUIComponent<UIComponentType::MenuBar>()
  {
    m_MainMenuBar->Render();
  }

  template<>
  void RenderUIComponent<UIComponentType::ObjectPanel>()
  {
    m_ObjectPanel->Render();
  }

  template<>
  void RenderUIComponent<UIComponentType::HierarchyPanel>()
  {
    m_HierarchyPanel->Render();
  }

  template<>
  void RenderUIComponent<UIComponentType::InspectorPanel>()
  {
    m_InspectorPanel->Render();
  }

private:

  void InitLayout()
  {
    InitMenuBar();
    InitProfilingPanel();
    InitSceneHierarchy();
    InitInspectorPanel();
  }

  void InitMenuBar()
  {
    m_MainMenuBar = std::make_shared<ImGuiMenuBar>("Main Menu Bar");

    m_MainMenuBar->AddSubMenu("File");
    m_MainMenuBar->AddSubMenu("Edit");
    m_MainMenuBar->AddSubMenu("View");
    m_MainMenuBar->AddSubMenu("Tool");
    m_MainMenuBar->AddSubMenu("Help");
    m_MainMenuBar->AddSubMenu("About");
    m_MainMenuBar->AddMenuItemToSubMenu("File", "Import", [&]() { /* 添加文件菜单的内容... */ });
    m_MainMenuBar->AddMenuItemToSubMenu("File", "Export", [&]() { /* 添加编辑菜单的内容... */ });
  }

  void InitProfilingPanel()
  {
    m_ObjectPanel = std::make_shared<ImGuiObjectPanel>("Game Object Panel");
  }

  void InitSceneHierarchy()
  {
    m_HierarchyPanel = std::make_shared<ImGuiTreeNodePanel>("Hierarchy Panel");

    m_HierarchyPanel->AddTreeNode("Root", [] { std::cout << "Clicked on Root" << std::endl; });
    m_HierarchyPanel->AddChildTreeNode("Root", "FolderA", [] { std::cout << "Clicked on FolderA" << std::endl; });
    m_HierarchyPanel->AddChildTreeNode("FolderA", "SubfolderA1", [] { std::cout << "Clicked on SubfolderA1" << std::endl; });
    m_HierarchyPanel->AddChildTreeNode("Root", "FolderB", [] { std::cout << "Clicked on FolderB" << std::endl; });
    m_HierarchyPanel->AddChildTreeNode("FolderB", "FileB1", [] { std::cout << "Clicked on FileB1" << std::endl; });
    m_HierarchyPanel->AddChildTreeNode("Root", "File1", [] { std::cout << "Clicked on File1" << std::endl; });
  }

  void InitInspectorPanel()
  {
    m_InspectorPanel = std::make_shared<ImGuiInspectorPanel>("Inspector Panel");
  }

private:
  std::shared_ptr<ImGuiMenuBar>			m_MainMenuBar;
  std::shared_ptr<ImGuiObjectPanel>		m_ObjectPanel;
  std::shared_ptr<ImGuiTreeNodePanel>		m_HierarchyPanel;
  std::shared_ptr<ImGuiInspectorPanel>		m_InspectorPanel;

  bool show_demo_window = false;

  GLFWwindow* windowHandle;
};

}
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
#include "UIComponent/ImGuiImagePanel.h"
#include "UIComponent/ImGuiFileExplorer.h"


namespace SPW
{
	class ImGuiImagePanel;

	class ImGuiManager
	{
	public:
		ImGuiManager() = default;

		void Init(GLFWwindow* window);
		void Begin();
		void End();
		void CleanUp();
		void ShowDemoWindow(bool show_demo_window);
		void EnableViewport() const;

		GLFWwindow* GetWindowHandle() const {return windowHandle;}

		std::shared_ptr<ImGuiObjectPanel>     GetEntityPanel() { return m_ObjectPanel; }
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

		template<>
		void RenderUIComponent<UIComponentType::Axis>()
		{
			m_InspectorPanel->Render();
		}

		template<>
		void RenderUIComponent<UIComponentType::Image>()
		{
			m_ImagePanel->Render();
		}

		template<>
		void RenderUIComponent<UIComponentType::FileExplorer>()
		{
			m_FileExplorer->Render();
		}

        void CreateAndRenderDockspacePanel(std::vector<std::shared_ptr<SPW::ImGuiPanel>> panels);

		void CreateImagePanel(GLuint tid)
		{
			m_ImagePanel = std::make_shared<ImGuiImagePanel>(tid);
		}

	private:

		void InitLayout()
		{
			InitIconManager();
			InitMenuBar();
			InitProfilingPanel();
			InitSceneHierarchy();
			InitInspectorPanel();
			InitFileExplorer();
		}

		void InitIconManager()
		{
			m_ImguiIconManager = std::make_unique<ImGuiIconManager>();
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
			m_ObjectPanel = std::make_shared<ImGuiObjectPanel>("Entity Panel");
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
			m_InspectorPanel = std::make_shared<ImGuiInspectorPanel>("Inspector Panel", m_ImguiIconManager.get());
		}

		void InitFileExplorer()
		{
			m_FileExplorer = std::make_shared<ImGuiFileExplorer>(m_ImguiIconManager.get());
		}


		//TODO:modify
	public:
		std::shared_ptr<ImGuiObjectPanel>		m_ObjectPanel;
		std::shared_ptr<ImGuiMenuBar>			m_MainMenuBar;
		std::shared_ptr<ImGuiTreeNodePanel>		m_HierarchyPanel;
		std::shared_ptr<ImGuiInspectorPanel>	m_InspectorPanel;
		std::shared_ptr<ImGuiImagePanel>	    m_ImagePanel;
		std::shared_ptr<ImGuiFileExplorer>      m_FileExplorer;

        //std::shared_ptr<>
	private:
		bool show_demo_window = false;
		std::shared_ptr<ImGuiDockSpace> m_DockspacePanel;
		std::unique_ptr<ImGuiIconManager> m_ImguiIconManager;

		GLFWwindow* windowHandle;

    };

}
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
#include "UIComponent/ImGuiEntityPanel.h"
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

		void RenderAllPanels() const { m_DockspacePanel->Render(); }
		void CreateImagePanel(uint64_t renderID);
		std::shared_ptr<ImGuiEntityPanel>     GetEntityPanel() { return m_ObjectPanel; }
		std::shared_ptr<ImGuiInspectorPanel>  GetInspectorPanel() { return m_InspectorPanel; }

	private:
		void InitLayout()
		{
			InitIconManager();
			InitMenuBar();
			InitProfilingPanel();
			InitSceneHierarchy();
			InitInspectorPanel();
			InitFileExplorer();

			m_DockspacePanel = std::make_shared<ImGuiDockSpace>();
			m_DockspacePanel->AddPanel(m_MainMenuBar);
			m_DockspacePanel->AddPanel(m_ObjectPanel);
			m_DockspacePanel->AddPanel(m_HierarchyPanel);
			m_DockspacePanel->AddPanel(m_InspectorPanel);
			m_DockspacePanel->AddPanel(m_FileExplorer);
		}

		void InitIconManager();

		void InitMenuBar();

		void InitProfilingPanel();

		void InitSceneHierarchy();

		void InitInspectorPanel();

		void InitFileExplorer()
		{
			m_FileExplorer = std::make_shared<ImGuiFileExplorer>(m_ImguiIconManager.get());
		}

	private:
		std::shared_ptr<ImGuiDockSpace>			m_DockspacePanel;
		std::shared_ptr<ImGuiEntityPanel>		m_ObjectPanel;
		std::shared_ptr<ImGuiMenuBar>			m_MainMenuBar;
		std::shared_ptr<ImGuiTreeNodePanel>		m_HierarchyPanel;
		std::shared_ptr<ImGuiInspectorPanel>	m_InspectorPanel;
		std::shared_ptr<ImGuiImagePanel>	    m_ImagePanel;
		std::shared_ptr<ImGuiFileExplorer>      m_FileExplorer;

		bool show_demo_window = false;
		GLFWwindow* windowHandle;
		std::unique_ptr<ImGuiIconManager>		m_ImguiIconManager;
    };

}
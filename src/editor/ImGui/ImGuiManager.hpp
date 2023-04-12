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
#include "ImGui/ImGuiMessageBox/ImGuiMessageBox.h"


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
		std::shared_ptr<ImGuiEntityPanel>     GetEntityPanel() { return m_EntityPanel; }
		std::shared_ptr<ImGuiInspectorPanel>  GetInspectorPanel() { return m_InspectorPanel; }

	private:
		void InitLayout();
		void InitIconManager();
		void InitMenuBar();
		void InitProfilingPanel();
		void InitSceneHierarchy();
		void InitInspectorPanel();
		void InitFileExplorer();

	private:
		std::shared_ptr<ImGuiDockSpace>			m_DockspacePanel;
		std::shared_ptr<ImGuiEntityPanel>		m_EntityPanel;
		std::shared_ptr<ImGuiMenuBar>			m_MainMenuBar;
		std::shared_ptr<ImGuiTreeNodePanel>		m_HierarchyPanel;
		std::shared_ptr<ImGuiInspectorPanel>	m_InspectorPanel;
		std::shared_ptr<ImGuiImagePanel>	    m_ImagePanel;
		std::shared_ptr<ImGuiFileExplorer>      m_FileExplorer;
		// std::shared_ptr<ImGuiMessageBox>		m_TestWindow;

		bool show_demo_window = false;
		GLFWwindow* windowHandle;
		std::unique_ptr<ImGuiIconManager>		m_ImguiIconManager;
    };

}

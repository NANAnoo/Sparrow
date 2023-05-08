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
#include "UIComponent/ImGuiFileDialogPanel.h"
#include "UIComponent/ImGuiInspectorPanel.h"
#include "UIComponent/ImGuiTreeNodePanel.h"
#include "UIComponent/ImGuiImagePanel.h"
#include "UIComponent/ImGuiFileExplorer.h"
#include "UIComponent/ImGuiProfilingPanel.h"
#include "ImGui/ImGuiMessageBox/ImGuiMessageBox.h"
#include "Asset/ResourceManager/ResourceManager.h"
#include "ImGui/IconsFontAwesome6.h"

namespace SPW
{
	class ImGuiImagePanel;

	class ImGuiManager
	{
	public:
		ImGuiManager(std::shared_ptr<Scene> scene)
		{
			m_Scene = scene;
		}
			

		void Init(GLFWwindow* window);
		void Begin();
		void End();
		void CleanUp();
		void ShowDemoWindow(bool show_demo_window);
		void EnableViewport() const;
		GLFWwindow* GetWindowHandle() const {return windowHandle;}

		void RenderAllPanels() const
		{
			m_DockspacePanel->Render();
			DisplayDialog();
		}
		void CreateImagePanel(uint64_t renderID);
		std::shared_ptr<ImGuiEntityPanel>     GetEntityPanel() { return m_EntityPanel; }
		std::shared_ptr<ImGuiInspectorPanel>  GetInspectorPanel() { return m_InspectorPanel; }

		void FileDialogCallBack_1();
		void FileDialogCallBack_2();
		void FileDialogCallBack_3();


		void DisplayDialog() const;
		void loadDefaultLayout() const;

	private:
		void InitLayout();
		void InitIconManager();
		void InitMenuBar();
		void InitEntityPanel();
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
		std::shared_ptr<ImGuiFileDialogPanel>   m_FileDialogPanel;
		std::shared_ptr<ImGuiProfilingPanel>    m_ProfilingPanel;
		std::shared_ptr<Scene>				    m_Scene;

		//file dialog panel
		SharedPtr<ImGuiFileDialog> file_dialog;

		std::unique_ptr<ImGuiMessageBox> importModel_MessageBox;
		std::unique_ptr<ImGuiMessageBox> textureCompression_MessageBox;

		bool show_demo_window = false;
		GLFWwindow* windowHandle;
		std::unique_ptr<ImGuiIconManager>		m_ImguiIconManager;
		std::uint32_t m_FileDialogID ;

    };

}

/*
* @date   2023-03-27
* @author dudu
* @brief  ${FILE_DESCRIPTION}
 */
#include "ImGuiManager.hpp"

namespace SPW
{
	void ImGuiManager::Init(GLFWwindow *window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(nullptr);

		windowHandle = window;

		InitLayout();
	}

	void ImGuiManager::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiManager::End()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiManager::CleanUp()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiManager::ShowDemoWindow(bool show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	void ImGuiManager::EnableViewport() const
	{
		ImGuiIO &io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(windowHandle);
		}
	}

	void ImGuiManager::CreateImagePanel(uint64_t renderID)
	{
		if (renderID)
		{
			m_ImagePanel = std::make_shared<ImGuiImagePanel>(renderID);
			m_DockspacePanel->AddPanel(m_ImagePanel);
		}
		else { std::cout << " Image Panel InValid!\n"; }
	}

	void ImGuiManager::InitLayout()
	{
		InitIconManager();
		InitMenuBar();
		InitProfilingPanel();
		InitSceneHierarchy();
		InitInspectorPanel();
		InitFileExplorer();
		InitFileDialogPanel();

		// m_TestWindow = std::make_shared<ImGuiMessageBox>("My Window", ImVec2(200, 100), ImVec2(100, 100));

		m_DockspacePanel = std::make_shared<ImGuiDockSpace>();
		m_DockspacePanel->AddPanel(m_MainMenuBar);
		m_DockspacePanel->AddPanel(m_EntityPanel);
		m_DockspacePanel->AddPanel(m_HierarchyPanel);
		m_DockspacePanel->AddPanel(m_InspectorPanel);
		m_DockspacePanel->AddPanel(m_FileExplorer);
		m_DockspacePanel->AddPanel(m_FileDialogPanel);

	}

	void ImGuiManager::InitIconManager()
	{
		m_ImguiIconManager = std::make_unique<ImGuiIconManager>();
	}

	void ImGuiManager::InitMenuBar()
	{
		m_MainMenuBar = std::make_shared<ImGuiMenuBar>("Main Menu Bar");

		m_MainMenuBar->AddSubMenu("File");
		m_MainMenuBar->AddSubMenu("Edit");
		m_MainMenuBar->AddSubMenu("View");
		m_MainMenuBar->AddSubMenu("Tool");
		m_MainMenuBar->AddSubMenu("Help");
		m_MainMenuBar->AddSubMenu("About");
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Import Model", [&]() { std::cout << "Clikecd on Import Model"; });
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Export Asset", [&]() {std::cout << "Clikecd on Export Asset"; });
	}

	void ImGuiManager::InitProfilingPanel()
	{
		m_EntityPanel = std::make_shared<ImGuiEntityPanel>("Entity Panel");
	}

	void ImGuiManager::InitSceneHierarchy()
	{
		m_HierarchyPanel = std::make_shared<ImGuiTreeNodePanel>("Hierarchy Panel");

		m_HierarchyPanel->AddTreeNode("Root", [] { std::cout << "Clicked on Root" << std::endl; });
		m_HierarchyPanel->AddChildTreeNode("Root", "FolderA", [] { std::cout << "Clicked on FolderA" << std::endl; });
		m_HierarchyPanel->AddChildTreeNode("FolderA", "SubfolderA1", [] { std::cout << "Clicked on SubfolderA1" << std::endl; });
		m_HierarchyPanel->AddChildTreeNode("Root", "FolderB", [] { std::cout << "Clicked on FolderB" << std::endl; });
		m_HierarchyPanel->AddChildTreeNode("FolderB", "FileB1", [] { std::cout << "Clicked on FileB1" << std::endl; });
		m_HierarchyPanel->AddChildTreeNode("Root", "File1", [] { std::cout << "Clicked on File1" << std::endl; });
	}

	void ImGuiManager::InitInspectorPanel()
	{
		m_InspectorPanel = std::make_shared<ImGuiInspectorPanel>("Inspector Panel", m_ImguiIconManager.get());
	}

	void ImGuiManager::InitFileExplorer()
	{
		m_FileExplorer = std::make_shared<ImGuiFileExplorer>(m_ImguiIconManager.get());
	}

	void ImGuiManager::InitFileDialogPanel()
	{
		m_FileDialogPanel = std::make_shared<ImGuiFileDialogPanel>();
	}
}

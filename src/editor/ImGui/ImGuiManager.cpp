/*
* @date   2023-03-27
* @author dudu
* @brief  ${FILE_DESCRIPTION}
 */
#include "ImGuiManager.hpp"

#include "Asset/Serializer/EntitySerializer.h"

namespace SPW
{
	void ImGuiManager::Init(GLFWwindow* window) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(nullptr);

		windowHandle = window;

		ImFont* default_font = io.Fonts->AddFontDefault();
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;  // This is important for merging the fonts
		icons_config.PixelSnapH = true;
		icons_config.OversampleH = icons_config.OversampleV = 1;
		icons_config.GlyphMinAdvanceX = 4.0f;
		icons_config.SizePixels = 16.f;
		icons_config.GlyphOffset = ImVec2(0.0f,3.0f);
		//io.Fonts->AddFontFromMemoryCompressedTTF(
		//	FONT_ICON_FILE_NAME_FAS,
		//	10, 16.f,
		//	&icons_config, icons_ranges);
		const char* font_path = "./resources/fonts/fa-solid-900.ttf";
		//const char* font_path = "./resources/fonts/fa-solid-900.ttf";
		io.Fonts->AddFontFromFileTTF(font_path, 16.0f, &icons_config, icons_ranges);

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
		InitEntityPanel();
		InitSceneHierarchy();
		InitInspectorPanel();
		InitFileExplorer();
		InitFileDialogPanel();
		m_ProfilingPanel = std::make_shared<ImGuiProfilingPanel>();

		// m_TestWindow = std::make_shared<ImGuiMessageBox>("My Window", ImVec2(200, 100), ImVec2(100, 100));

		m_DockspacePanel = std::make_shared<ImGuiDockSpace>();
		m_DockspacePanel->AddPanel(m_MainMenuBar);
		m_DockspacePanel->AddPanel(m_EntityPanel);
		m_DockspacePanel->AddPanel(m_HierarchyPanel);
		m_DockspacePanel->AddPanel(m_InspectorPanel);
		m_DockspacePanel->AddPanel(m_FileExplorer);
		m_DockspacePanel->AddPanel(m_FileDialogPanel);
		m_DockspacePanel->AddPanel(m_ProfilingPanel);
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
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Save Scene", [&]() {SPW::EntitySerializer::SaveScene(m_Scene); });
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Load Scene", [&]() {std::cout << "Clikecd on Export Asset"; });
	}

	void ImGuiManager::InitEntityPanel()
	{
		m_EntityPanel = std::make_shared<ImGuiEntityPanel>("Entity Panel");
	}

	void ImGuiManager::InitSceneHierarchy()
	{
		m_HierarchyPanel = std::make_shared<ImGuiTreeNodePanel>(ICON_FA_LIST"  Hierarchy Panel");

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

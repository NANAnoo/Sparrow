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
		//io.IniFilename = "./default.ini";
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

		//file dialog panel
		file_dialog = std::make_shared<ImGuiFileDialog>();

		importModel_MessageBox = std::make_unique<ImGuiMessageBox>("Import Model", "file", "Import Model Sucessed!", std::vector{ "OK" }, false);
		textureCompression_MessageBox = std::make_unique<ImGuiMessageBox>("textureCompression_MessageBox Model", "file", "textureCompression_MessageBox Model Sucessed!", std::vector{ "OK" }, false);

		m_FileDialogID = -1;

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
		loadDefaultLayout();
		InitIconManager();
		InitMenuBar();
		InitEntityPanel();
		InitSceneHierarchy();
		InitInspectorPanel();
		InitFileExplorer();
		InitLogPanel();
		m_ProfilingPanel = std::make_shared<ImGuiProfilingPanel>();

		// m_TestWindow = std::make_shared<ImGuiMessageBox>("My Window", ImVec2(200, 100), ImVec2(100, 100));

		m_DockspacePanel = std::make_shared<ImGuiDockSpace>();
		m_DockspacePanel->AddPanel(m_MainMenuBar);
		m_DockspacePanel->AddPanel(m_EntityPanel);
		m_DockspacePanel->AddPanel(m_HierarchyPanel);
		m_DockspacePanel->AddPanel(m_InspectorPanel);
		m_DockspacePanel->AddPanel(m_FileExplorer);
		m_DockspacePanel->AddPanel(m_ProfilingPanel);
		m_DockspacePanel->AddPanel(m_LogPanel);

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
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Import Model", [&]() {FileDialogCallBack_1(); });
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Import Asset", [&]() {FileDialogCallBack_2();  });
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Image Compression", [&]() {FileDialogCallBack_3();  });

		//using fileDiaglogCallback = std::function<void()>;

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

	void ImGuiManager::InitLogPanel()
	{
		m_LogPanel = std::make_shared<ImGuiLog>();
	}

	void ImGuiManager::FileDialogCallBack_1()
	{
		file_dialog->OpenDialog("ChooseFileDlgKey", "Choose File", "*.*", ".");
		m_FileDialogID = 1;
	}

	void ImGuiManager::FileDialogCallBack_2()
	{
		file_dialog->OpenDialog("ChooseFileDlgKey", "Choose File", "*.*", ".");
		m_FileDialogID = 2;
	}

	void ImGuiManager::FileDialogCallBack_3()
	{
		file_dialog->OpenDialog("ChooseFileDlgKey", "Choose File", "*.*", ".");
		m_FileDialogID = 3;
	}

	void ImGuiManager::DisplayDialog() const
	{
		if (file_dialog->Display("ChooseFileDlgKey"))
		{
			if (file_dialog->IsOk())
			{
				std::string file_path = file_dialog->GetFilePathName();
				// Do something with the file_path
				std::string filePathName = file_dialog->GetFilePathName();
				std::string fileName = file_dialog->GetCurrentFileName();

				std::string extension = FileSystem::ToFsPath(fileName).extension().string();
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);


				if(m_FileDialogID == 1)
				{
					if (extension == ".gltf" || extension == ".fbx" || extension == ".obj")
					{
						if (AssetManager::ImportModel(filePathName))
						{
							importModel_MessageBox->trigger_flag = true;
						}
					}
				}

				if(m_FileDialogID == 2)
				{
					if (extension == ".json" || extension == ".asset")
					{
						auto asset_data = AssetManager::LoadAsset(filePathName);
						ResourceManager::getInstance()->m_AssetDataMap[asset_data.assetName] = asset_data;
					}
				}

				if (m_FileDialogID == 3)
				{
					if (extension == ".png" || extension == ".jpg" || extension == ".jepg")
					{
						auto data = AssetManager::LoadTextureData(filePathName);
						if (AssetManager::CompressImage(std::move(data), filePathName))
						{
							textureCompression_MessageBox->trigger_flag = true;
						}
					}
				}
				
			}
			file_dialog->Close();
		}
	}

	void ImGuiManager::loadDefaultLayout() const{
		std::ifstream default_ini("./default.ini");
		std::ofstream imgui_ini("./imgui.ini");

		if (!default_ini.is_open() || !imgui_ini.is_open()) {
			std::cerr << "Error: Could not open ini files." << std::endl;
			return;
		}

		std::string line;
		while (std::getline(default_ini, line)) {
			imgui_ini << line << std::endl;
		}

		default_ini.close();
		imgui_ini.close();
	}

	//void ImGuiManager::SetLog(std::string log)
	//{
	//	m_Log =log;
	//}

}

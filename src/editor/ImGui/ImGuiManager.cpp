/*
* @date   2023-03-27
* @author dudu
* @brief  ${FILE_DESCRIPTION}
 */
#include "ImGuiManager.hpp"

#include "Asset/Serializer/EntitySerializer.h"
#include "dirent/dirent.h"

namespace SPW
{
	void ImGuiManager::Init(GLFWwindow* window)
	{
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

		// HWND hwnd = (HWND)ImGui::GetIO().ImeWindowHandle;

		m_Window = window;

		ImFont* default_font = io.Fonts->AddFontDefault();
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;  // This is important for merging the fonts
		icons_config.PixelSnapH = true;
		icons_config.OversampleH = icons_config.OversampleV = 1;
		icons_config.GlyphMinAdvanceX = 4.0f;
		icons_config.SizePixels = 16.f;
		icons_config.GlyphOffset = ImVec2(0.0f,3.0f);

		std::string fontPath = Config::k_EngineRoot + "fonts/fa-solid-900.ttf";
		const char* fontPath_cstr = fontPath.c_str();
		io.Fonts->AddFontFromFileTTF(fontPath_cstr, 16.0f, &icons_config, icons_ranges);

		//file dialog panel
		m_FileDialog = std::make_shared<ImGuiFileDialog>();

		m_ImportModelMessageBox = std::make_unique<ImGuiMessageBox>("Import Model", "file", "Import Model Sucessed!", std::vector{ "OK" }, false);
		m_TextureCompressionMessageBox = std::make_unique<ImGuiMessageBox>("m_TextureCompressionMessageBox Model", "file", "m_TextureCompressionMessageBox Model Sucessed!", std::vector{ "OK" }, false);

// 		m_FileDialogID = -1;

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
			glfwMakeContextCurrent(m_Window);
		}
	}

	void ImGuiManager::CreateImagePanel(uint64_t renderID)
	{
		if (renderID)
		{
			m_ImagePanel = std::make_shared<ImGuiImagePanel>(renderID);
			m_Dockspace->AddPanel(m_ImagePanel);
		}
		else { std::cout << " Image Panel InValid!\n"; }
	}

	void ImGuiManager::InitLayout()
	{
		LoadDefaultLayout();
		InitIconManager();
		InitMenuBar();
		InitEntityPanel();
		InitInspectorPanel();
		InitFileExplorer();
		InitImagePanel();
		InitProfilingPanel();

		m_Dockspace = std::make_shared<ImGuiDockSpace>();
		m_Dockspace->AddPanel(m_MainMenuBar);
		m_Dockspace->AddPanel(m_EntityPanel);
		m_Dockspace->AddPanel(m_InspectorPanel);
		m_Dockspace->AddPanel(m_FileExplorer);
		m_Dockspace->AddPanel(m_ProfilingPanel);
		m_Dockspace->AddPanel(m_ImagePanel);
	}

	void ImGuiManager::InitImagePanel()
	{
		m_ImagePanel = std::make_shared<ImGuiImagePanel>(0);
	}

	void ImGuiManager::InitProfilingPanel()
	{
		m_ProfilingPanel = std::make_shared<ImGuiProfilingPanel>();
	}

	void ImGuiManager::InitIconManager()
	{
		m_ImguiIconManager = std::make_unique<ImGuiIconManager>();
	}

	void ImGuiManager::InitMenuBar()
	{
		m_MainMenuBar = std::make_shared<ImGuiMenuBar>("Main Menu Bar");

		m_MainMenuBar->AddSubMenu("File");
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Save Scene", [&]() {SaveSceneCallback(); });
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Load Scene", [&]() {LoadSceneCallback(); });
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Import Model", [&]() {ImportModelCallback(); });
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Import Audio", [&]() {ImportAudioCallback(); });
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Load Asset", [&]() {LoadAssetCallback();  });
		m_MainMenuBar->AddMenuItemToSubMenu("File", "Setup Script Entry", [&]() {SetupScriptEntryCallback();  });

//		m_MainMenuBar->AddSubMenu("Edit");
//		m_MainMenuBar->AddSubMenu("View");

		m_MainMenuBar->AddSubMenu("Tool");
		m_MainMenuBar->AddMenuItemToSubMenu("Tool", "Image Compression", [&]() {ImageCompressedCallback();  });

		m_MainMenuBar->AddSubMenu("Window");
		m_MainMenuBar->AddMenuItemToSubMenu("Window", "Save Editor Layout", [&]() {SaveEditorLayoutCallback(); });

		m_MainMenuBar->AddSubMenu("Help");
		m_MainMenuBar->AddMenuItemToSubMenu("Help", "People Should Help Themselves", [&]() { });

		m_MainMenuBar->AddSubMenu("About");
		m_MainMenuBar->AddMenuItemToSubMenu("About", "Open Configuration File", [&]() { OpenConfigCallback();  });
	}

	void ImGuiManager::InitEntityPanel()
	{
		m_EntityPanel = std::make_shared<ImGuiEntityPanel>("Entity Panel");
	}


	void ImGuiManager::InitInspectorPanel()
	{
		m_InspectorPanel = std::make_shared<ImGuiInspectorPanel>("Inspector Panel", m_ImguiIconManager.get());
	}

	void ImGuiManager::InitFileExplorer()
	{
		m_FileExplorer = std::make_shared<ImGuiFileExplorer>(m_ImguiIconManager.get());
	}

	// void ImGuiManager::InitLogPanel()
	// {
	// 	m_LogPanel = std::make_shared<ImGuiLog>();
	// }

	void ImGuiManager::SaveSceneCallback()
	{
		if (!fs::exists(Config::k_WorkingProjectScenes))
			SPW::FileSystem::SPWCreateDirectory(Config::k_WorkingProjectScenes);

		m_FileDialog->OpenDialog("ChooseDirDlgKey", "Save Scene", nullptr, Config::k_WorkingProjectScenes);
		m_Feature = FeatureType::SaveScene;
	}

	void ImGuiManager::LoadSceneCallback()
	{
		if (!fs::exists(Config::k_WorkingProjectScenes))
			FileSystem::SPWCreateDirectory(Config::k_WorkingProjectScenes);

		m_FileDialog->OpenDialog("Universal_FileDialog", "Load Scene", "*.*", Config::k_WorkingProjectScenes);
		m_Feature = FeatureType::LoadScene;
	}

	void ImGuiManager::SetupScriptEntryCallback()
	{
		if (!fs::exists(Config::k_WorkingProjectScripts))
			FileSystem::SPWCreateDirectory(Config::k_WorkingProjectScripts);

		m_FileDialog->OpenDialog("Universal_FileDialog", "Script Entry", "*.*", Config::k_WorkingProjectScripts);
		m_Feature = FeatureType::SetupScriptEntry;
	}

	void ImGuiManager::ImportModelCallback()
	{
		m_FileDialog->OpenDialog("Universal_FileDialog", "Import Model", "*.*", FileSystem::GetUserHomeDir().string());
		m_Feature = FeatureType::ImportModel;
	}

	void ImGuiManager::LoadAssetCallback()
	{
		if (!fs::exists(Config::k_WorkingProjectAssets))
			FileSystem::SPWCreateDirectory(Config::k_WorkingProjectAssets);

		m_FileDialog->OpenDialog("Universal_FileDialog", "Load Asset", "*.*", Config::k_WorkingProjectAssets);
		m_Feature = FeatureType::LoadAsset;
	}

	void ImGuiManager::ImageCompressedCallback()
	{
		if (!fs::exists(Config::k_WorkingProjectAssets))
			FileSystem::SPWCreateDirectory(Config::k_WorkingProjectAssets);

		m_FileDialog->OpenDialog("Universal_FileDialog", "Compress Image", "*.*", Config::k_WorkingProjectAssets);
		m_Feature = FeatureType::ImageCompression;
	}

	void ImGuiManager::OpenConfigCallback()
	{
		const std::filesystem::path config_path = FileSystem::GetUserHomeDir().append("./.config/sparrow.toml");

		if (!fs::exists(config_path))
			std::cerr << "Error: Why dont u have the config file ? " << std::endl;

		std::string command = "code " + config_path.string();

		if (std::system(command.c_str()))
			std::cerr << "Error: Failed to open the file in Visual Studio Code" << std::endl;
	}

	void ImGuiManager::ImportAudioCallback()
	{
		std::string openPath;
		if (!fs::exists(Config::k_WorkingProjectRoot + "Sounds/"))
			openPath = Config::k_WorkingProjectRoot;
		else
			openPath = Config::k_WorkingProjectRoot + "Sounds/";

		m_FileDialog->OpenDialog("Universal_FileDialog", "Import Audio", "*.*", openPath);
		m_Feature = FeatureType::ImportAudio;
	}

	void ImGuiManager::DisplayDialog()
	{
		if (m_FileDialog->Display("Universal_FileDialog"))
		{
			if (m_FileDialog->IsOk())
			{
				std::string filePath = m_FileDialog->GetFilePathName();
				std::string fileName = m_FileDialog->GetCurrentFileName();
				std::string extension = FileSystem::ToFsPath(fileName).extension().string();
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

				if ( m_Feature == FeatureType::ImportModel )
				{
					if (extension == ".gltf" || extension == ".fbx" || extension == ".obj")
					{
						if (AssetManager::ImportModel(filePath))
						{
							m_ImportModelMessageBox->trigger_flag = true;
						}
					}
					else
					{
						std::cout << "Not Support ! \n";
					}
				}

				if ( m_Feature == FeatureType::SetupScriptEntry)
				{
					toml::table cfg = ConfigManager::GetConfigContext();

					if (extension == ".lua")
					{
						FileSystem::ResolveSlash(filePath);
						toml::table scriptEntries = toml::table
						{
							// get file name | filepath
							{"Entry", filePath}
						};
						cfg.insert_or_assign("DefaultScript", scriptEntries);
						ConfigManager::WriteDefaultScript(cfg);
					}
					else
					{
						std::cout << "Not Support ! \n";
					}
				}

				if ( m_Feature == FeatureType::LoadAsset )
				{
					if (extension == ".json" || extension == ".asset")
					{
						auto asset_data = AssetManager::LoadAsset(filePath);
						ResourceManager::getInstance()->m_AssetDataMap[asset_data.assetName] = asset_data;
					}
					else
					{
						std::cout << "Not Support ! \n";
					}

				}

				if (m_Feature == FeatureType::ImageCompression )
				{
					if (extension == ".png" || extension == ".jpg" || extension == ".jepg")
					{
						auto data = AssetManager::LoadTextureData(filePath);
						if (AssetManager::CompressImage(std::move(data), filePath))
						{
							m_TextureCompressionMessageBox->trigger_flag = true;
						}
					}
					else
					{
						std::cout << "Not Support ! \n";
					}

				}

				if (m_Feature == FeatureType::ImportAudio)
				{
					if (extension == ".wav" || extension == ".mp3")
					{
						// auto data = AssetManager::ImportAudio(filePathName);
						if (AssetManager::ImportAudio(filePath))
						{
							m_TextureCompressionMessageBox->trigger_flag = true;
						}
					}
					else
					{
						std::cout << "Not Support ! \n";
					}
				}

				if (m_Feature == FeatureType::LoadScene)
				{
					if (extension == Extension::JsonEx || extension == Extension::SceneEx)
					{
						m_Scene->clearAllEntity();
						m_EntityPanel->ClearItems();
						if (EntitySerializer::LoadScene(m_Scene, filePath))
						{
							m_ImportModelMessageBox->trigger_flag = true;
						}
					}
					else
					{
						std::cout << "Not Support ! \n";
					}
				}

			}
			m_FileDialog->Close();
		}

		if (m_FileDialog->Display("ChooseDirDlgKey"))
		{
			if (m_FileDialog->IsOk())
			{
				selectedFile = ImGuiFileDialog::Instance()->GetFilePathName();
				showInputBox = true;
			}
			m_FileDialog->Close();
		}

		if (showInputBox)
		{
			static char m_PendingName[256] = "";
			ImGui::Text("Enter the name of the scene: ");
			ImGui::SameLine();
			ImGui::InputText("Save Name", m_PendingName, sizeof(m_PendingName));

			if (ImGui::Button("OK"))
			{
				std::string saveName = m_PendingName;
				EntitySerializer::SaveScene(m_Scene, selectedFile, saveName);
				showInputBox = false;
				memset(m_PendingName, 0, sizeof(m_PendingName));
			}
		}
	}

	void ImGuiManager::SaveEditorLayoutCallback() 
	{
		SaveEditorLayout();
	}

	void ImGuiManager::SaveEditorLayout() const
	{
		const std::string editorLayoutPath = Config::k_EngineRoot + "/fonts/EditorLayout.ini";
		std::ifstream currConfigFile("./imgui.ini");
		std::ofstream writeConfigFile(editorLayoutPath);

		if (!writeConfigFile.is_open() || !currConfigFile.is_open())
		{
			std::cerr << "Error: Could not open ini files." << std::endl;
			return;
		}

		std::string line;
		while (std::getline(currConfigFile, line))
		{
			writeConfigFile << line << std::endl;
		}

		currConfigFile.close();
		writeConfigFile.close();
	}

	void ImGuiManager::LoadDefaultLayout() const
	{
// 		ImGui::GetIO().IniFilename = Config::k_EngineRoot + "/fonts/EditorLayout.ini";
		const std::string editorLayoutPath = Config::k_EngineRoot + "/fonts/EditorLayout.ini";

		std::ifstream default_ini(editorLayoutPath);
		std::ofstream imgui_ini("./imgui.ini");

		if (!default_ini.is_open() || !imgui_ini.is_open()) 
		{
			std::cerr << "Error: Could not open ini files." << std::endl;
			return;
		}

		std::string line;
		while (std::getline(default_ini, line)) 
		{
			imgui_ini << line << std::endl;
		}

		default_ini.close();
		imgui_ini.close();
	}

	void ImGuiManager::Render()
	{
		Begin();
		//----------------------------------------------------------------------------------------

		m_ImagePanel->SetupCurrImageID(m_Scene->m_renderSystem.lock()->getTextureID());

		// m_ImagePanel = std::make_shared<ImGuiImagePanel>(m_Scene->m_renderSystem.lock()->getTextureID());
		// m_Dockspace->AddPanel(m_ImagePanel);

		// CreateImagePanel(m_Scene->m_renderSystem.lock()->getTextureID());

		RenderAllPanels();
		//----------------------------------------------------------------------------------------
		m_EntityPanel->SetActiveScene(m_Scene);
		m_InspectorPanel->SetActiveScene(m_Scene);

		m_Scene->forEachEntity<IDComponent>([this](const Entity& e)
			{
				const auto component_name = e.component<NameComponent>()->getName();
				const auto component_id = e.component<IDComponent>()->getID().toString();
				m_EntityPanel->AddMenuItem(component_id, component_name, [&, e]()
					{
						m_InspectorPanel->SetSelectedGameObject(e);
					}
				);
			});
		//----------------------------------------------------------------------------------------
		End();
		EnableViewport();
	}

}

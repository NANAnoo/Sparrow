//
// Created by dudu on 2023/4/20.
//

#include "ConfigManager.h"

namespace SPW
{
	namespace Config
	{
		std::string k_EngineRoot;
		std::string k_EngineShaderLib;
		std::string k_EngineLualib;

		std::string k_TempalteProjectRoot;
		std::string k_WorkingProjectRoot;
		std::string k_WorkingProjectAssets;
		std::string k_WorkingProjectSounds;
		std::string k_WorkingProjectScenes;
		std::string k_WorkingProjectShaders;
		std::string k_WorkingProjectScripts;
		std::string k_WorkingProjectUI;
	}

	bool ConfigManager::Boost()
	{
		if (ReadConfig())
			std::cout << "Successfully read config file" << std::endl;
		else 
			ReadConfig();

		// copy engine shaders
		FileSystem::MountPath(Config::k_EngineShaderLib, SPW::Config::k_WorkingProjectShaders);

		// copy template project root with default asset
		FileSystem::MountPath(Config::k_TempalteProjectRoot, SPW::Config::k_WorkingProjectRoot);

		// copy sounds
		FileSystem::MountPath(Config::k_EngineRoot + "sounds/", SPW::Config::k_WorkingProjectSounds);

		// copy scripts
		FileSystem::MountPath(Config::k_EngineRoot + "scripts/", SPW::Config::k_WorkingProjectScripts);

		// copy UI resources TODO ui path
		FileSystem::MountPath(Config::k_EngineRoot + "texture/", SPW::Config::k_WorkingProjectUI);

		// reference source code lualib
		if (std::filesystem::exists(SPW::Config::k_EngineLualib))
			std::cout << "[CONFIG]::lualib exsits! \n";

		return true;
	}

	bool ConfigManager::ReadConfig()
	{
		// std::string orgPath = FileSystem::GetUserHomeDir() + "./.config/����.toml"
//        std::string = "./.config/sparrow.toml";
        // std::filesystem::path  config_path= std::filesystem::path("./.config/sparrow.toml");
        const std::filesystem::path config_path = FileSystem::GetUserHomeDir().append("./.config/sparrow.toml");

		if (fs::exists(config_path))
		{
			// if path exists, directly from sparrow.toml
			toml::table config = toml::parse_file(config_path.u8string());

			std::cout << "Boost Sparrow Eninge: " << config["Sparrow"] << std::endl;

			{
				auto ret = config["Engine"]["RootPath"].as<std::string>();
				if (ret)
					Config::k_EngineRoot = std::string(*ret);
				else
				{
					WriteDefaultConfig();
					return false;
				}
			}

			{
				auto ret = config["Engine"]["RuntimePath"].as<std::string>();
				if (ret)
					Config::k_EngineLualib = std::string(*ret) + "/src/LuaLib/";
				else
				{
					WriteDefaultConfig();
					return false;
				}
			}

			{
				auto ret = config["Engine"]["SparrowTemplate"].as<std::string>();
				if(ret)
					Config::k_TempalteProjectRoot = std::string(*ret);
				else
				{
					WriteDefaultConfig();
					return false;
				}
			}


			// new shader lib from Engine to 
			Config::k_EngineShaderLib = Config::k_EngineRoot + "shaders/";


			std::unordered_map<std::string, std::string> project_paths;

			if (config.contains("Projects"))
			{
				toml::table& projects_table = *config["Projects"].as_table();

				for (const auto& [project_name, project_value] : projects_table)
				{
					if (project_value.is_string())
					{
						project_paths.emplace(project_name, project_value.as_string()->get());
					}
				}
			}

			if (!project_paths.empty())
			{
				Config::k_WorkingProjectRoot	= project_paths.begin()->second;
				Config::k_WorkingProjectAssets	= project_paths.begin()->second + "Assets/";
				Config::k_WorkingProjectSounds	= project_paths.begin()->second + "Sounds/";
				Config::k_WorkingProjectScenes	= project_paths.begin()->second + "Scenes/";
				Config::k_WorkingProjectShaders = project_paths.begin()->second + "Shaders/";
				Config::k_WorkingProjectScripts = project_paths.begin()->second + "Scripts/";
				Config::k_WorkingProjectUI		= project_paths.begin()->second + "UI/";
			}

			std::cout << "Engine Root: " << Config::k_EngineRoot << std::endl;
			std::cout << "Template Root: " << Config::k_TempalteProjectRoot << std::endl;
			std::cout << "Working Project Root: " << Config::k_WorkingProjectRoot << std::endl;
			std::cout << "Working Project Assets: " << Config::k_WorkingProjectAssets << std::endl;
			std::cout << "Working Project Scenes: " << Config::k_WorkingProjectScenes << std::endl;

			return true;
		}
		else
		{
			//FileSystem::CreateDirectory(config_path.parent_path().string());

			std::filesystem::path absolute_engine_path = std::filesystem::absolute("./resources/");
			std::string absolute_engine = absolute_engine_path.string();
			FileSystem::ResolveSlash(absolute_engine);
			Config::k_EngineRoot = absolute_engine;

			std::filesystem::path absolute_template_path = std::filesystem::absolute("./SparrowTemplate/");
			std::string absolute_template = absolute_template_path.string();
			FileSystem::ResolveSlash(absolute_template);
			Config::k_TempalteProjectRoot = absolute_template;
			Config::k_WorkingProjectRoot = absolute_template;
			Config::k_WorkingProjectAssets = absolute_template + "Assets/";
			Config::k_WorkingProjectScenes = absolute_template + "Scenes/";

			WriteConfig(absolute_template);

			return true;
		}

		return false;
	}

	bool ConfigManager::WriteDefaultConfig()
	{
		std::filesystem::path config_file_dir = FileSystem::GetUserHomeDir().append("./.config/");
		FileSystem::CreateDirectory(config_file_dir);

		toml::table engine_meta = toml::table
		{
			{"Version", "0.1.1"}
		};

		std::filesystem::path absolute_runtime_path = std::filesystem::absolute("./");
		std::string absolute_runtime = absolute_runtime_path.string();
		SPW::FileSystem::ResolveSlash(absolute_runtime);

		std::filesystem::path absolute_engine_path = std::filesystem::absolute("./resources/");
		std::string absolute_engine = absolute_engine_path.string();
		SPW::FileSystem::ResolveSlash(absolute_engine);

		std::filesystem::path absolute_template_path = std::filesystem::absolute("./SparrowTemplate/");
		std::string absolute_template = absolute_template_path.string();
		SPW::FileSystem::ResolveSlash(absolute_template);

		// SparrowEngine - SparrowTemplate
		toml::table engine_path = toml::table
		{
			{"RootPath", absolute_engine},
			{"RuntimePath", absolute_runtime},
			{"SparrowTemplate", absolute_template}
		};


		toml::table projects = toml::table
		{
			// project_name, project_path
			{"SparrowTemplate", absolute_template},
		};

		toml::table config_table = toml::table
		{
			{"Sparrow", engine_meta},
			{"Engine", engine_path},
			{"Projects", projects},
		};

		std::ofstream config{config_file_dir.append("sparrow.toml")};
		if (config.is_open())
		{
			config << config_table;
			return true;
		}
		else
		{
			std::cerr << "Failed to open config file." << std::endl;
			return false;
		}
	}

	bool ConfigManager::WriteConfig(const std::string& curr_path)
	{

		std::filesystem::path config_file_dir = SPW::FileSystem::GetUserHomeDir().append("./.config/");
		SPW::FileSystem::CreateDirectory(config_file_dir);

		toml::table engine_meta = toml::table
		{
			{"Version", "0.1.1"}
		};

		std::filesystem::path absolute_engine_path = std::filesystem::absolute("./resources/");
		std::string absolute_engine = absolute_engine_path.string();
		SPW::FileSystem::ResolveSlash(absolute_engine);

		std::filesystem::path absolute_template_path = std::filesystem::absolute("./SparrowTemplate/");
		std::string absolute_template = absolute_template_path.string();
		SPW::FileSystem::ResolveSlash(absolute_template);

		// SparrowEngine - SparrowTemplate
		toml::table engine_path = toml::table
		{
			{"RootPath", absolute_engine},
			{"SparrowTemplate", absolute_template}
		};


		toml::table projects = toml::table
		{
			// project_name, project_path
			{"SparrowTemplate", curr_path},
		};

		toml::table config_table = toml::table
		{
			{"Sparrow", engine_meta},
			{"Engine", engine_path},
			{"Projects", projects},
		};

		std::ofstream config{config_file_dir.append("sparrow.toml")};
		if (config.is_open())
		{
			config << config_table;
			return true;
		}
		else
		{
			std::cerr << "Failed to open config file." << std::endl;
			return false;
		}
	}
}

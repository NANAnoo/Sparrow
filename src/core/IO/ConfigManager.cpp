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
		std::string k_WorkingProjectScenes;
	}

	bool ConfigManager::ReadConfig()
	{
		const std::filesystem::path config_path = FileSystem::GetUserHomeDir() + "./.config/sparrow.toml";

		if (fs::exists(config_path))
		{
			// if path exists, directly from sparrow.toml
			toml::table config = toml::parse_file(config_path.string());

			std::cout << "Boost Sparrow Eninge: " << config["Sparrow"] << std::endl;

			Config::k_EngineRoot = std::string(*config["Engine"]["RootPath"].as<std::string>());
//			Config::k_EngineShaderLib = Config::k_EngineRoot + "/";
			Config::k_TempalteProjectRoot = std::string(*config["Engine"]["SparrowTemplate"].as<std::string>());

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
				Config::k_WorkingProjectRoot = project_paths.begin()->second;
				Config::k_WorkingProjectAssets = project_paths.begin()->second + "Assets/";
				Config::k_WorkingProjectScenes = project_paths.begin()->second + "Scenes/";
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

	bool ConfigManager::WriteConfig()
	{
		std::string config_file_dir = SPW::FileSystem::GetUserHomeDir() + "./.config/";
		FileSystem::CreateDirectory(config_file_dir);

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
			{"SparrowTemplate", "// TODO NEW ABSOLTE PATH"},
		};

		toml::table config_table = toml::table
		{
			{"Sparrow", engine_meta},
			{"Engine", engine_path},
			{"Projects", projects},
		};

		std::ofstream config{config_file_dir + "sparrow.toml"};
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
		std::string config_file_dir = SPW::FileSystem::GetUserHomeDir() + "./.config/";
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

		std::ofstream config{config_file_dir + "sparrow.toml"};
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

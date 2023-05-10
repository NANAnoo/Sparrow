/*
* @date   2023-04-20
* @author dudu
* @brief  ${FILE_DESCRIPTION}
*/
#pragma once
#include "FileSystem.h"
#include <toml++/toml.h>

namespace SPW
{

	namespace Config
	{
		// engine resource location
		extern std::string k_EngineRoot;
		// engine resource/shaders location
		extern std::string k_EngineShaderLib;
		// source code src/LuaLib location
		extern std::string k_EngineLualib;

		// SparrowTemplate
		extern std::string k_TempalteProjectRoot;


		// Project
		extern std::string k_WorkingProjectRoot;
		// Project/Assets
		extern std::string k_WorkingProjectAssets;
		// Project/Sounds
		extern std::string k_WorkingProjectSounds;
		// Project/Scenes
		extern std::string k_WorkingProjectScenes;
		// Project/Shaders
		extern std::string k_WorkingProjectShaders;
		// Project/Scripts
		extern std::string k_WorkingProjectScripts;

		// engine entrence script
		// proj   entrence script
	}


	struct EngineConfig
	{
		std::string spw_version;
		std::string spw_root;
		std::string spw_template;
	};

	struct ProjectConfig
	{
		std::string proj_name;
		std::string proj_path;
	};

	class ConfigManager
	{
	public:
		static bool Boost();
		static bool ReadConfig();



		static bool WriteConfig();
		static bool WriteConfig(const std::string& curr_path);
	};
}


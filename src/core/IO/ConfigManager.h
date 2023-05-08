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
		extern std::string k_EngineRoot;
		extern std::string k_EngineShaderLib;
		extern std::string k_EngineLualib;

		// engine shaders // copy 
		// engine scripts // ref

		extern std::string k_TempalteProjectRoot;


		// Project
		extern std::string k_WorkingProjectRoot;
		extern std::string k_WorkingProjectAssets;
		extern std::string k_WorkingProjectScenes;

		// sounds copy
		// / shaders copy/
		// scritps copy

		// engine entrence script
		// proj   entrence script

		// 
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

		static bool ReadConfig();



		static bool WriteConfig();
		static bool WriteConfig(const std::string& curr_path);
	};
}


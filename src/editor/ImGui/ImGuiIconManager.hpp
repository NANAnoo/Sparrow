/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include <fstream>
#include <glad/glad.h>
#include "ImGui/ImGuiDefinitions.h"
// #include <stb/stb_image.h>

#include "IO/FileSystem.h"

#include "IO/SOIL2/SOIL2.h"
#include "IO/SOIL2/stb_image.h"

#include "Asset/AssetManager/AssetManager.h"

namespace SPW
{
	inline const std::unordered_map<std::string, std::string> k_IconLib
		{
			{ "file", "./resources/icons/doc.dds" },
			{ "folder", "./resources/icons/file.dds" },
			{ "obj", "./resources/icons/obj.dds" },
			{ "music", "./resources/icons/music.dds" }
		};

	class ImGuiIconManager
	{
	public:
		ImGuiIconManager()
		{
			for (const auto& [k, v]: k_IconLib)
			{
				GenerateIconID(v);
			}
		}

		int64_t GetLibIcon(const char* name)
		{
			return GenerateIconID(k_IconLib.find(name)->second);
		}

		int64_t GenerateIconID(const std::string& filename)
		{
			// do not load the lib texture twice
			if (m_IconIDMap.contains(filename))
			{ return m_IconIDMap[filename]; }

			// if the file is a dds file, load it by SOIL2 directly
			if (FilePath(filename).extension() == ".dds")
			{
				int64_t textureId = AssetManager::LoadCompressedImage(filename);
				m_IconIDMap[filename] = textureId;
				return textureId;
			}
			else
			{
				int64_t textureId = AssetManager::LoadRawImage(filename);
				m_IconIDMap[filename] = textureId;
				return textureId;
			}
		}

		std::unordered_map<std::string, int64_t> m_IconIDMap;
	};

}

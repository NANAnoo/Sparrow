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
			{ "music", "./resources/icons/music.dds" },
			{ "fbx", "./resources/icons/fbx.dds" },
			{ "anim", "./resources/icons/anim.dds" },
			{ "cpp", "./resources/icons/cpp.dds" },
			{ "gltf", "./resources/icons/gltf.dds" },
			{ "h", "./resources/icons/h.dds" },
			{ "json", "./resources/icons/json.dds" },
			{ "lua", "./resources/icons/lua.dds" },
			{ "mesh", "./resources/icons/mesh.dds" }
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

		int64_t LoadImage(const std::string& file_path) {
			int width, height, channels;
			unsigned char* image_data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
			if (image_data == NULL) {
				return 0;
			}

			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			GLenum format;
			switch (channels)
			{
			case 1:
				format = GL_R;
				break;
			case 2:
				format = GL_RG;
				break;
			case 3:
				format = GL_RGB;
				break;
			case 4:
				format = GL_RGBA;
				break;
			default:
				stbi_image_free(image_data);
				return 0;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
			glBindTexture(GL_TEXTURE_2D, 0);

			stbi_image_free(image_data);
			return texture;
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
				//int64_t textureId = LoadImage(filename);
				//int64_t textureId = static_cast<int64_t>(reinterpret_cast<intptr_t>(textureId1));
				m_IconIDMap[filename] = textureId;
				return textureId;
			}
		}

		std::unordered_map<std::string, int64_t> m_IconIDMap;
	};

}

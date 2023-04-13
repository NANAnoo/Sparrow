/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include <fstream>
#include <glad/glad.h>
#include "ImGui/ImGuiDefinitions.h"
#include <stb/stb_image.h>
#include "IO/FileSystem.h"

#include "SOIL2/SOIL2.h"
#include "SOIL2/common.hpp"

#ifndef GL_EXT_texture_compression_s3tc
#define GL_EXT_texture_compression_s3tc 1
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif

namespace SPW
{
	inline const std::unordered_map<std::string, std::string> k_IconLib
	{
		{"file", "./resources/icons/doc.dds"},
		{"folder", "./resources/icons/file.dds"},
		{"obj", "./resources/icons/obj.dds"},
		{"music", "./resources/icons/music.dds"}
	};

	class ImGuiIconManager
	{
	public:
		ImGuiIconManager()
		{
			for(const auto&[k, v] : k_IconLib)
			{
				GenerateTextureID(v);
			}
		}

		int64_t GetLibIcon(const char* name)
		{
			return GenerateTextureID(k_IconLib.find(name)->second);
		}

		struct ImageData
		{
			int width;
			int height;
			int numChannels;
			unsigned char* data;
		};

		static std::vector<char> ReadFile(const std::string& filename)
		{
			std::ifstream file(filename, std::ios::binary | std::ios::ate);
			if (!file)
			{
				std::cerr << "Failed to open file: " << filename << std::endl;
				return {};
			}

			size_t fileSize = static_cast<size_t>(file.tellg());
			file.seekg(0, std::ios::beg);

			std::vector<char> buffer(fileSize);
			file.read(buffer.data(), fileSize);

			return buffer;
		}

		static int64_t LoadCompressedImage(const std::string& filename)
		{
			int64_t ret = 0;
			ret = SOIL_load_OGL_texture
			(
				filename.c_str(),
				SOIL_LOAD_AUTO,
				ret,
				SOIL_FLAG_DDS_LOAD_DIRECT
			);
			return ret;
		}

		static int64_t LoadRawImage(const std::string& filename)
		{
			return SOIL_load_OGL_texture
			(
				filename.c_str(),
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);
		}

		static ImageData LoadTextureData(const std::string& filename)
		{
			int width, height, numChannels;
			unsigned char* data = stbi_load(filename.c_str(), &width, &height, &numChannels, 0);

			if (!data)
			{
				std::cerr << "Failed to load texture from file: " << filename << std::endl;
				return { 0, 0, 0, nullptr};
			} // return empty ImageData

			return { width, height, numChannels, data }; // return ImageData with loaded texture data
		}

		static bool ImageCompression(ImageData&& imageData, const std::string& filename)
		{
			std::string newfilepath = FilePath(filename).replace_extension(".dds").string();
			const char* newfilename = newfilepath.c_str();
			/* save that image as another type */
			int save_result = SOIL_save_image
			(
				newfilename,
				SOIL_SAVE_TYPE_DDS,
				imageData.width, imageData.height, imageData.numChannels,
				imageData.data
			);

			if (save_result == 0) {
				std::cerr << "Error saving DDS file: " << SOIL_last_result() << std::endl;
				return true;
			}

			std::cout << "DDS file saved successfully." << std::endl;
			return false;
		}

		int64_t GenerateTextureID(const std::string& filename)
	    {
			// dont load the lib texture twice
	        if(m_IconIDMap.contains(filename)) { return m_IconIDMap[filename]; }

			// if the file is a dds file, load it by SOIL2 directly
			if(FilePath(filename).extension() == ".dds")
			{
				int64_t textureId = LoadCompressedImage(filename);
				m_IconIDMap[filename] = textureId;
				return textureId;
			}
			else
			{
				int64_t textureId = LoadRawImage(filename);
				m_IconIDMap[filename] = textureId;
				return textureId;
			}

	    }


		std::unordered_map<std::string, int64_t> m_IconIDMap;
//		ImVec2 imageSize = ImVec2(40, 40);
	};

}

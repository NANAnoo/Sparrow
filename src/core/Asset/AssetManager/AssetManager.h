/*
 * @date   2023-04-12
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include <string>
#include <memory>

#include "Asset/ModelLoader/ModelLoader.h"
#include "Asset/Asset.hpp"
#include "Asset/AssetData/AssetData.h"
// #include "Utils/SingletonBase.h"

namespace SPW
{

#ifndef GL_EXT_texture_compression_s3tc
#define GL_EXT_texture_compression_s3tc 1
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif

	struct ImageData
	{
		int width;
		int height;
		int numChannels;
		unsigned char* data;
	};

	class AssetManager
	{
	public:
		static bool LoadAsset(const std::string& path, AssetData& assetData);
		static AssetData LoadAsset(const std::string& path);

		static bool SaveAsset(std::unique_ptr<AssetData>&& model_data, std::string org_path);

		static bool ImportModel(const std::string& path);
		static bool ImportAudio(const std::string& path);

		static bool CompressImage(ImageData&& imageData, const std::string& filename);

		// directly return the textureID in GLuint
		static int64_t LoadRawImage(const std::string& filename);
		static int64_t LoadCompressedImage(const std::string& filename);
		static uint32_t LoadCubemapImages(const std::vector<std::string>& faces);

		// return textureData by stbi
		static ImageData LoadTextureData(const std::string& filename);
	};
} // SPW


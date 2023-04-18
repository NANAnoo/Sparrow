//
// Created by dudu on 2023/4/12.
//

#include "AssetManager.h"
#include "IO/FileSystem.h"

// #define STB_IMAGE_IMPLEMENTATION
#include "IO/SOIL2/SOIL2.h"
#include "IO/SOIL2/stb_image.h"
// #include <stb/stb_image.h>

namespace SPW
{
	// TODO choose path by user, return the data for the mesh component.
	bool AssetManager::LoadAsset(const std::string& path, AssetData& assetData)
	{
		std::ifstream is_Scene(path); // TODO: Select the asset file to Load by GUI operations
		cereal::JSONInputArchive ar(is_Scene);

		ar(
			cereal::make_nvp("assetID", assetData.assetID),
			cereal::make_nvp("assetName", assetData.assetName),
			cereal::make_nvp("assetPath", assetData.path),
			cereal::make_nvp("meshURI", assetData.meshURI),
			cereal::make_nvp("materials", assetData.materials),
			cereal::make_nvp("textures", assetData.textures)
		);

		{
			std::ifstream
				mesh_bin(FilePath(path).parent_path().string() + "/" + assetData.meshURI + ".mesh", std::ios::binary);
			cereal::BinaryInputArchive ar(mesh_bin);
			ar(cereal::make_nvp(assetData.meshURI, assetData.meshes));
		}

		return true;
	}

	AssetData AssetManager::LoadAsset(const std::string& path)
	{
		AssetData assetData;
		LoadAsset(path, assetData);
		return assetData;
	}

	// save data from the model imported.
	bool AssetManager::SaveAsset(std::unique_ptr<AssetData>&& model_data, std::string org_path)
	{
		std::string absolute_modelDir = FileSystem::JoinPaths(FileRoots::k_Assets, model_data->assetName);
		FileSystem::CreateDirectory(absolute_modelDir);

		// Update Model Path
		model_data->path = FileSystem::JoinPaths(absolute_modelDir, model_data->assetName) + ".json";

		// write file with absolute file path
		std::ofstream of_file(model_data->path); // TODO .asset, use json to view the file currently
		cereal::JSONOutputArchive ar(of_file);

		// copy texture files into /Assets/Textures...
		// 1. Create Texture Directory
		if (!model_data->textures.empty())
		{
			std::string textureDir = FileSystem::JoinPaths(absolute_modelDir, "Textures/");
			FileSystem::CreateDirectory(textureDir);

			// 2. Loop Textures and Copy Files into Texture Directory
			for (auto& [k, v] : model_data->textures)
			{
				// std::string rgo = org_path;
				std::string src = std::filesystem::path(org_path).parent_path().string();
				FileSystem::ResolveSlash(src);
				
				std::string sourceFilePath = src + v;
				std::string destinationFilePath(textureDir + FileSystem::ToFsPath(v).filename().string());
				FileSystem::CopyFile(sourceFilePath, destinationFilePath);

				// 3. Update Texture Path To Relative
				v = FileSystem::ToRelativePath(destinationFilePath, FileRoots::k_Root);
			}
		}

		ar(
			cereal::make_nvp("assetID", model_data->assetID),
			cereal::make_nvp("assetName", model_data->assetName),
			cereal::make_nvp("assetPath", FileSystem::ToRelativePath(model_data->path, SPW::FileRoots::k_Root)),
			cereal::make_nvp("meshURI", model_data->meshURI),
			cereal::make_nvp("materials", model_data->materials),
			cereal::make_nvp("textures", model_data->textures)
		);

		{
			std::ofstream
				mesh_bin(FileSystem::JoinPaths(absolute_modelDir, model_data->meshURI) + ".mesh", std::ios::binary);
			cereal::BinaryOutputArchive archive(mesh_bin);
			archive(cereal::make_nvp(model_data->meshURI, model_data->meshes));
		}

		return true;
	}

	bool AssetManager::ImportModel(const std::string& path)
	{
		std::unique_ptr<AssetData> model_data;
		try
		{
			model_data = ModelLoader::LoadModel(path);
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
			return false;
		}

		return SaveAsset(std::move(model_data), path) ? true : false;
	}

	int64_t AssetManager::LoadCompressedImage(const std::string& filename)
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

	uint32_t AssetManager::LoadCubemapImages(const std::vector<std::string>& faces)
	{
		/* load 6 images into a new OpenGL cube map, forcing RGB */
		uint32_t tex_cube = SOIL_load_OGL_cubemap
		(
			faces[0].c_str(),
			faces[1].c_str(),
			faces[2].c_str(),
			faces[3].c_str(),
			faces[4].c_str(),
			faces[5].c_str(),
			SOIL_LOAD_RGB,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS
		);

		/* check for an error during the load process */
		if (0 == tex_cube)
		{
			printf("SOIL loading error: '%s'\n", SOIL_last_result());
		}

		return tex_cube;
	}

	int64_t AssetManager::LoadRawImage(const std::string& filename)
	{
		/* load an image file directly as a new OpenGL texture */
		int width, height, channels;
		unsigned char* image = SOIL_load_image
		(
			filename.c_str(),
			&width, &height, &channels,
			SOIL_LOAD_L
		);

		/* load an image file directly as a new OpenGL texture */
		int64_t tex_2d = SOIL_load_OGL_texture
		(
			filename.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			/*SOIL_FLAG_MIPMAPS | */SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

		/* check for an error during the load process */
		if (0 == tex_2d)
		{
			printf("SOIL loading error: '%s'\n", SOIL_last_result());
		}

		return tex_2d;
	}

	bool AssetManager::CompressImage(ImageData&& imageData, const std::string& filename)
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

		if (save_result == 0)
		{
			std::cerr << "Error saving DDS file: " << SOIL_last_result() << std::endl;
			return true;
		}

		std::cout << "DDS file saved successfully." << std::endl;
		return false;
	}

	ImageData AssetManager::LoadTextureData(const std::string& filename)
	{
		int width, height, numChannels;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &numChannels, 0);

		if (!data)
		{
			std::cerr << "Failed to load texture from file: " << filename << std::endl;
			return {0, 0, 0, nullptr};
		} // return empty ImageData

		return {width, height, numChannels, data}; // return ImageData with loaded texture data
	}
} // SPW

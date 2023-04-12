//
// Created by dudu on 2023/4/12.
//

#include "AssetManager.h"
#include "IO/FileSystem.h"

namespace SPW
{
    // TODO choose path by user, return the data for the mesh component.
    bool AssetManager::LoadAsset(const std::string& path, AssetData& assetData)
    {
        std::ifstream is_Scene(path); // TODO: Select the asset file to Load by GUI operations
        cereal::JSONInputArchive ar(is_Scene);
        
        ar(
            cereal::make_nvp("assetID", assetData.assetID),
            cereal::make_nvp("assetPath", assetData.path),
            cereal::make_nvp("meshURI", assetData.meshURI),
            cereal::make_nvp("materials", assetData.materials),
            cereal::make_nvp("textures", assetData.textures)
        );

        {
            std::ifstream mesh_bin(FilePath(path).parent_path().string() + "/" + assetData.meshURI + ".mesh", std::ios::binary);
            cereal::BinaryInputArchive ar(mesh_bin);
            ar(cereal::make_nvp(assetData.meshURI, assetData.meshes));
        }

	    return true;
    }

    // save data from the mesh component.
    bool AssetManager::SaveAsset(std::unique_ptr<ModelData>&& model_data)
    {
        std::string absolute_modelDir = FileSystem::JoinPaths(FileRoots::k_Assets, model_data->name);
        FileSystem::CreateDirectory(absolute_modelDir);

        // Update Model Path
        model_data->path = FileSystem::JoinPaths(absolute_modelDir, model_data->name) + ".json";

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
                std::string destinationFilePath(textureDir + FileSystem::ToFsPath(v).filename().string());
                FileSystem::CopyFile(v, destinationFilePath);

                // 3. Update Texture Path To Relative
                v = FileSystem::ToRelativePath(destinationFilePath, FileRoots::k_Root);
            }
        }

        ar(
            cereal::make_nvp("assetID", model_data->assetID),
            cereal::make_nvp("assetPath", FileSystem::ToRelativePath(model_data->path, SPW::FileRoots::k_Root)),
            cereal::make_nvp("meshURI", model_data->meshURI),
            cereal::make_nvp("materials", model_data->materials),
            cereal::make_nvp("textures", model_data->textures)
        );

        {
            std::ofstream mesh_bin(FileSystem::JoinPaths(absolute_modelDir, model_data->meshURI) + ".mesh", std::ios::binary);
            cereal::BinaryOutputArchive archive(mesh_bin);
            archive(cereal::make_nvp(model_data->meshURI, model_data->meshes));
        }

        return true;
	}

	bool AssetManager::ImportModel(const std::string& path)
	{
        std::unique_ptr<ModelData> model_data;
        try
        {
	        model_data = ModelLoader::LoadModel(path);
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
			return false;
		}

        return SaveAsset(std::move(model_data)) ? true : false;
	}
} // SPW
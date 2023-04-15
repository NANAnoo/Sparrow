#pragma once
#include "Utils/SingletonBase.h"
#include "Model/Model.h"
#include "Render/shader.h"
#include "Asset/AssetManager/AssetManager.h"
#include "Asset/AssetData/AssetData.h"

#include <filesystem>

namespace SPW
{
	class ResourceManager : public SingletonBase<ResourceManager> 
	{
	public:
		std::shared_ptr<Model> LoadModel(const std::filesystem::path& filePath);
		using textureLoadCallback = std::function<void(int width, int height, int bpp, unsigned char*)>;
		void LoadTextureScope(bool flip, const std::filesystem::path& filePath, const textureLoadCallback &callback);

	public:
		std::unordered_map<std::string, AssetData> m_AssetDataMap;
	};

	// class UniResourceManager
	// {
	// public:
	// 	UniResourceManager()  = default;
	// 	~UniResourceManager() = default;
	//
	// 	// name - AssetData
	// 	std::unordered_map<std::string, AssetData> m_AssetDataMap;
	// };
}
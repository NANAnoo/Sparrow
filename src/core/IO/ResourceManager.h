#pragma once
#include "Utils/SingletonBase.h"
#include "Model/Model.h"
#include "Render/shader.h"
#include <filesystem>
#include "Model/Animation/AnimDefinitions.h"

namespace SPW
{
	class ResourceManager : public SingletonBase<ResourceManager> 
	{
	public:
		std::shared_ptr<Model> LoadModel(const std::filesystem::path& filePath);
		using textureLoadCallback = std::function<void(int width, int height, int bpp, unsigned char*)>;
		void LoadTextureScope(bool flip, const std::filesystem::path& filePath, const textureLoadCallback &callback);

		static std::unique_ptr<Skeleton> LoadAnimation(const std::filesystem::path& filePath);
	};

}

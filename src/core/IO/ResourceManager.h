#pragma once
#include "Utils/SingletonBase.h"
#include "Model/Model.h"
#include "Render/shader.h"
#include "Model/Animation/Skeleton.h"
#include <filesystem>

namespace SPW
{
	class ResourceManager : public SingletonBase<ResourceManager> 
	{
	public:
		std::shared_ptr<Model> LoadModel(const std::filesystem::path& filePath);
		using textureLoadCallback = std::function<void(int width, int height, int bpp, unsigned char*)>;
		void LoadTextureScope(bool flip, const std::filesystem::path& filePath, const textureLoadCallback &callback);

		[[nodiscard]] std::shared_ptr<Skeleton> LoadAnimation(const std::filesystem::path& filePath);
	};

}
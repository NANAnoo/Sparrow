#pragma once
#include "Utils/SingletonBase.h"
#include "Model/Model.h"
#include "Animation/Skeleton.h"
#include "Render/shader.h"
#include <filesystem>

namespace SPW
{
	class ResourceManager : public SingletonBase<ResourceManager> 
	{
	public:
		[[nodiscard]] std::shared_ptr<Model> LoadModel(const std::filesystem::path& filePath);

		[[nodiscard]] std::shared_ptr<Skeleton> LoadAnimation(const std::filesystem::path& filePath);
	};

}
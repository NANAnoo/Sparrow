#pragma once
#include "Utils/SingletonBase.h"
#include "Model/Model.h"
#include "Render/shader.h"
#include <filesystem>

namespace SPW
{
	class ResourceManager : public SingletonBase<ResourceManager> 
	{
	public:
		std::shared_ptr<Model> LoadModel(const std::filesystem::path& filePath);
		void LoadTexture();
	};

}
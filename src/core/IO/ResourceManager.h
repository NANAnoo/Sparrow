#pragma once
#include "Utils/SingletonBase.h"
#include "Model/Model.h"
#include "Render/shader.h"
#include <filesystem>

namespace SPW
{
	class Model;

	enum class ResourceType
	{
		MODEL,
		SHADER,
		TEXTURE,
	};

	class ResourceManager : public SPW::SingletonBase<ResourceManager> 
	{
	public:
		std::shared_ptr<Model> LoadModel(const std::filesystem::path& filePath);

	private:
	};

}
#pragma once
#include "Utils/SingletonBase.h"
#include "Model/Model.h"
#include "Render/shader.h"
#include <filesystem>

namespace SPW
{


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


	auto GetResouces() const
	{
		return m_Resources;
	}

	auto GetModels() const
	{
		return m_Models;
	}


private:
	float m_AGenertor = 1.0f;

	std::unordered_map<std::string, ResourceType> m_Resources;
	std::unordered_map<std::string, std::shared_ptr<Model>> m_Models;
};

}
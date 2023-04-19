/*
* @date   2023-03-31
* @author dudu
* @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include "Asset/Asset.hpp"
#include "Asset/AssetData/AssetData.h"
#include "Asset/AssetData/MaterialData.h"
#include "Asset/AssetData/MeshData.h"
#include "Asset/AssetData/ModelData.h"
#include "Asset/AssetData/Mesh.h"

namespace SPW
{
	class ModelLoader
	{
	public:
		static std::unique_ptr<AssetData> LoadModel(const std::string& filename);
		static std::unique_ptr<Skeleton> LoadAnimation(const std::filesystem::path& filePath);
	};
}

/*
 * @date   2023-04-12
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include <string>
#include <memory>
#include "Asset/ModelLoader/ModelLoader.h"
#include "Asset/Asset.hpp"
#include "Asset/AssetData/AssetData.h"

namespace SPW
{
	class AssetManager
	{
	public:
		static bool LoadAsset(const std::string& path, AssetData& assetData);
		static bool SaveAsset(std::unique_ptr<ModelData>&& model_data);

		static bool ImportModel(const std::string& path);
	};
} // SPW


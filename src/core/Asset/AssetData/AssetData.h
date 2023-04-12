/*
 * @date   2023-04-09
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include <glm/glm.hpp>

#include "MaterialData.h"
#include "Asset/Asset.hpp"
#include "Model/Mesh.h"

namespace SPW
{
	struct AssetData
	{
        std::string                                     assetID;
        std::string                                     path;
        std::string                                     meshURI;
        std::vector<Mesh>                               meshes;
        std::vector<MaterialData>                       materials;
        std::unordered_map<std::string, std::string>    textures;

		// template <class Archive>
		// void serialize(Archive& ar)
		// {
		// 	ar(
  //               cereal::make_nvp("assetID", assetID),
  //               cereal::make_nvp("assetPath", path),
  //               cereal::make_nvp("meshURI", meshURI),
  //               cereal::make_nvp("materials", materials),
  //               cereal::make_nvp("textures", textures)
  //           );
		// }

	};
}

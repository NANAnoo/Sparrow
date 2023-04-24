/*
 * @date   2023-04-09
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once


#include <string>
#include <fstream>

// ----------- CEREAL Headers included -------------------
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

namespace SPW
{
	enum class AssetType
	{
		StaticModel,
		AnimatedModel,
		Mesh,
		Material,
		AnimationClip,
		Skeleton,
		Texture2D,
	};

	struct Asset
	{
		std::string ID;
		AssetType   type;
		std::string name;
		std::string path;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("id",   ID),
				cereal::make_nvp("type", type),
				cereal::make_nvp("name", name),
				cereal::make_nvp("path", path)
			);
		}

	};

}

#include "AssetDataSupport.inl"

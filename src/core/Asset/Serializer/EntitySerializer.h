/*
* @date   2023-04-09
* @author dudu
* @brief  ${FILE_DESCRIPTION}
*/
#pragma once

#include "EcsFramework/Scene.hpp"
#include "Asset/Asset.hpp"
#include "IO/ConfigManager.h"
#include "Render/RenderGraphManager.h"

namespace SPW
{
	struct EntityNode
	{
		std::string uuid;
		std::string name;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("uuid", uuid), cereal::make_nvp("name", name));
		}
	};

	class EntitySerializer
	{
	public:
		EntitySerializer() = default;
		~EntitySerializer() = default;
		EntitySerializer(const EntitySerializer& ts) = delete;
		EntitySerializer(EntitySerializer&& ts) = delete;

		// save prefab
		static bool SaveEntity(const std::shared_ptr<Entity>& e, const std::string& filePath = "");

		// load prefab
		static bool LoadEntity(std::shared_ptr<Scene>& scene, const std::string& filePath = "");

		// save scene snapshot
		static bool SaveScene(const std::shared_ptr<Scene>& scene, const std::string& filePath, const std::string& username = "");

		// load scene snapshot
		static bool LoadScene(const std::shared_ptr<Scene>& scene, const std::string& path );
	};
} // namespace SPW

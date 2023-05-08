/*
* @date   2023-04-09
* @author dudu
* @brief  ${FILE_DESCRIPTION}
*/
#pragma once

#include "EcsFramework/Scene.hpp"
#include "Asset/Asset.hpp"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Component/AnimationComponent/AnimationComponent.h"
#include "IO/FileSystem.h"
#include "Asset/ResourceManager/ResourceManager.h"
#include "IO/ConfigManager.h"

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

		static bool SaveScene(const std::shared_ptr<Scene>& scene, const std::string& filePath = "")
		{
			fs::path savePath;
			if (filePath.length() > 0)
			{
				savePath = filePath;
			}
			else
			{
				savePath = Config::k_WorkingProjectScenes + "/scene.json";
			}

			std::unordered_map<std::string, CameraComponent> cameraComponents;
			std::unordered_map<std::string, PointLightComponent> pointLightComponents;
			std::unordered_map<std::string, DirectionalLightComponent> directionalLightComponents;
			std::unordered_map<std::string, TransformComponent> transformComponents;
			std::unordered_map<std::string, MeshComponent> meshComponents;
			std::unordered_map<std::string, AnimationComponent> animationComponents;
			std::vector<EntityNode> entityNodes;

			scene->forEachEntity<IDComponent>([&](const Entity& e)
			{
				const std::string uuid_str = e.component<IDComponent>()->getID().toString();

				entityNodes.emplace_back(EntityNode{uuid_str, e.component<NameComponent>()->getName()});

				if (e.has<CameraComponent>())
				{
					cameraComponents[uuid_str] = *e.component<CameraComponent>();
				}
				if (e.has<PointLightComponent>())
				{
					pointLightComponents[uuid_str] = *e.component<PointLightComponent>();
				}
				if (e.has<DirectionalLightComponent>())
				{
					directionalLightComponents[uuid_str] = *e.component<DirectionalLightComponent>();
				}
				if (e.has<TransformComponent>())
				{
					transformComponents[uuid_str] = *e.component<TransformComponent>();
				}
				if (e.has<MeshComponent>())
				{
					meshComponents[uuid_str] = *e.component<MeshComponent>();
				}
				if (e.has<AnimationComponent>())
				{
					animationComponents[uuid_str] = *e.component<AnimationComponent>();
				}
			});

			std::ofstream of_file(savePath);
			cereal::JSONOutputArchive ar(of_file);
			ar( cereal::make_nvp("entityNodes", entityNodes ));
			ar( cereal::make_nvp("cameraComponents", cameraComponents ));
			ar( cereal::make_nvp("pointLightComponents", pointLightComponents ));
			ar( cereal::make_nvp("directionalComponents", directionalLightComponents ));
			ar( cereal::make_nvp("transformComponents", transformComponents ));
			ar( cereal::make_nvp("meshComponents", meshComponents ));
			ar( cereal::make_nvp("animationComponents", animationComponents ));

			return true;
		}


		static bool LoadScene(std::shared_ptr<Scene>& scene, const std::string& path = "")
		{
			// Read from .json
			std::ifstream is_Scene(Config::k_WorkingProjectScenes + "/scene.json"); // TODO: Select the asset file to Load by GUI operations
			cereal::JSONInputArchive ar(is_Scene);


			std::unordered_map<std::string, TransformComponent> transformComponents;
			std::unordered_map<std::string, CameraComponent> cameraComponents;
			std::unordered_map<std::string, PointLightComponent> pointLightComponents;
			std::unordered_map<std::string, DirectionalLightComponent> directionalLightComponents;
			std::unordered_map<std::string, MeshComponent> meshComponents;
			std::unordered_map<std::string, AnimationComponent> animationComponents;
			std::vector<EntityNode> entityNodes;

			ar(cereal::make_nvp("transformComponents", transformComponents));
			ar(cereal::make_nvp("entityNodes", entityNodes));
			ar(cereal::make_nvp("cameraComponents", cameraComponents));
			ar(cereal::make_nvp("pointLightComponents", pointLightComponents));
			ar(cereal::make_nvp("directionalComponents", directionalLightComponents));
			ar(cereal::make_nvp("meshComponents", meshComponents));
			ar(cereal::make_nvp("animationComponents", animationComponents));


			// std::vector<EntityNode> entityNodes;
			for (auto& node : entityNodes)
			{
				auto e = scene->createEntity(node.name, UUID(node.uuid));
				for(auto&[id, data] : transformComponents)
				{
					if (node.uuid == id)
					{
						e->emplace<TransformComponent>(data);
					}
				}

				for(auto&[id, data] : pointLightComponents)
				{
					if (node.uuid == id)
					{
						e->emplace<PointLightComponent>(data);
					}
				}

				for(auto&[id, data] : directionalLightComponents)
				{
					if (node.uuid == id)
					{
						e->emplace<DirectionalLightComponent>(data);
					}
				}

				for(auto&[id, data] : cameraComponents)
				{
					if (node.uuid == id)
					{
						e->emplace<CameraComponent>(data);
					}
				}

				for(auto&[id, data] : meshComponents)
				{
					if (node.uuid == id)
					{
						const auto& rm = ResourceManager::getInstance();

						{
							auto assetData = AssetManager::LoadAsset(Config::k_TempalteProjectRoot + data.assetPath);
							rm->m_AssetDataMap.emplace(assetData.assetName, assetData);
						}

						auto mesh = e->emplace<MeshComponent>(data);

						std::cout << rm->activeCameraID.toString() << std::endl;
						mesh->bindCamera = (rm->activeCameraID);
						mesh->assetName = data.assetName;
						mesh->assetID = data.assetID;
						mesh->assetPath = data.assetPath;
						mesh->ready = false;

						if (mesh->assetName == "sand_cube")
						{
							mesh->bindRenderGraph = rm->m_RenderGraph["pbr_with_PDshadow"]->graph_id;
							mesh->modelSubPassPrograms[rm->m_ModelToScreenNodes["pbr_shadow_lighting_node"]->pass_id] = rm->m_ShaderMap["pbr_light_shadow_tiled_desc"].uuid;
						}
						else
						{
							mesh->bindRenderGraph = rm->m_RenderGraph["pbr_with_PDshadow"]->graph_id;
							mesh->modelSubPassPrograms[rm->m_ModelRepeatPassNodes["p_shadowmap_node"]->pass_id] = rm->m_ShaderMap["p_shadow_desc"].uuid;
							mesh->modelSubPassPrograms[rm->m_ModelRepeatPassNodes["d_shadowmap_node"]->pass_id] = rm->m_ShaderMap["d_shadow_desc"].uuid;
							mesh->modelSubPassPrograms[rm->m_ModelToScreenNodes["pbr_shadow_lighting_node"]->pass_id] = rm->m_ShaderMap["pbr_light_shadow_desc"].uuid;
						}

						if(mesh->assetName == "skybox")
						{
							mesh->bindRenderGraph = rm->m_RenderGraph["skybox"]->graph_id;
							mesh->modelSubPassPrograms[rm->m_ModelToScreenNodes["skybox"]->pass_id] = rm->m_ShaderMap["skybox"].uuid;
						}
					}
				}

				for(auto&[id, data] : animationComponents)
				{
					if (node.uuid == id)
					{
						const auto& rm = ResourceManager::getInstance();

						auto anim = e->emplace<AnimationComponent>(data.assetName);

						anim->swapCurrentAnim("dragon_idle");

						auto animMesh = e->component<MeshComponent>();

						animMesh->bindRenderGraph = rm->m_RenderGraph["pbr_with_PDshadow"]->graph_id;
						animMesh->modelSubPassPrograms[rm->m_ModelRepeatPassNodes["p_shadowmap_node"]->pass_id] = rm->m_ShaderMap["p_ani_shadow_desc"].uuid;
						animMesh->modelSubPassPrograms[rm->m_ModelRepeatPassNodes["d_shadowmap_node"]->pass_id] = rm->m_ShaderMap["d_ani_shadow_desc"].uuid;
						animMesh->modelSubPassPrograms[rm->m_ModelToScreenNodes["pbr_shadow_lighting_node"]->pass_id] = rm->m_ShaderMap["pbr_ani_light_shadow_desc"].uuid;
					}
				}
			}


			return true;
		}
	};
} // namespace SPW

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

					const auto& assetData = ResourceManager::getInstance()->m_AssetDataMap.at(e.component<MeshComponent>()->assetName);

					std::string path = Config::k_WorkingProjectRoot + e.component<MeshComponent>()->assetPath;
					std::ofstream outAsset(path);
					cereal::JSONOutputArchive ar(outAsset);
					ar(
						cereal::make_nvp("assetID", assetData.assetID),
						cereal::make_nvp("assetName", assetData.assetName),
						cereal::make_nvp("assetPath", e.component<MeshComponent>()->assetPath),                                  
						cereal::make_nvp("meshURI", assetData.meshURI),
						cereal::make_nvp("materials", assetData.materials),
						cereal::make_nvp("textures", assetData.textures)
					);

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
			of_file.close();

			return true;
		}


		static bool LoadScene(const std::shared_ptr<Scene>& scene, const std::string& path = "")
		{
			// Read from .json
			std::string new_path = path;
			FileSystem::ResolveSlash(new_path);
			//{};
			std::ifstream is_Scene(new_path); // TODO: Select the asset file to Load by GUI operations
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
							mesh->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
							mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(SPW::kFloorGBufferShader).uuid;
						}
						else
						{
							mesh->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
							mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kPointShadowNode)] = GET_SHADER_DESC(SPW::kPointShadowShader).uuid;
							mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kDirectionalShadowNode)] = GET_SHADER_DESC(SPW::kDirectionalShadowShader).uuid;
							mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(SPW::kGBufferShader).uuid;
						}

						if(mesh->assetName == "skybox")
						{
							mesh->bindRenderGraph = GET_RENDER_GRAPH(SPW::kSkyBoxRenderGraph	);
							mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kSkyboxNode	)] = GET_SHADER_DESC(SPW::kSkyBoxShader).uuid;
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

						animMesh->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
						animMesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kPointShadowNode)] = GET_SHADER_DESC(SPW::kAniPointShadowShader).uuid;
						animMesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kDirectionalShadowNode)] = GET_SHADER_DESC(SPW::kAniDirectionalShadowShader).uuid;
						animMesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(SPW::kAniGBufferShader).uuid;
					}
				}
			}


			return true;
		}
	};
} // namespace SPW

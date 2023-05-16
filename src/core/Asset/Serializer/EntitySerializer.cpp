//
// Created by dudu on 2023/4/9.
//

#include "EntitySerializer.h"
#include "EcsFramework/Component/COmponentTypes.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Component/AnimationComponent/AnimationComponent.h"
#include "IO/FileSystem.h"
#include "Asset/ResourceManager/ResourceManager.h"


namespace SPW {
	bool EntitySerializer::SaveEntity(const std::shared_ptr<Entity>& e, const std::string& filePath)
	{
		FileSystem::SPWCreateDirectory(Config::k_WorkingProjectAssets + "Prefabs/");

		std::string saveDir = Config::k_WorkingProjectAssets + "Prefabs/";
		fs::path savePath;
		if (!filePath.empty())
		{
			savePath = filePath;
		}
		else
		{
			std::string savename = e->getName();
			while (std::filesystem::exists(saveDir + savename + Extension::PrefabEx))
			{
				savename += "(1)";
			}
			savePath = saveDir + savename + Extension::PrefabEx;
		}

		std::ofstream outPrefab(savePath);
		cereal::JSONOutputArchive ar(outPrefab);
			
		EntityNode node
		{
			e->component<IDComponent>()->getID().toString(),
			e->component<NameComponent>()->getName()
		};

		std::unordered_map<std::string, bool> mask;

		ar(cereal::make_nvp("EntityNode", node));

		if (e->has<CameraComponent>())
		{
			mask[ComponentTypeToString(ComponentType::CameraComponent)] = true;
		}
		if (e->has<PointLightComponent>())
		{
			mask[ComponentTypeToString(ComponentType::PointLightComponent)] = true;
		}
		if (e->has<DirectionalLightComponent>())
		{
			mask[ComponentTypeToString(ComponentType::DirectionalLightComponent)] = true;
		}
		if (e->has<TransformComponent>())
		{
			mask[ComponentTypeToString(ComponentType::TransformComponent)] = true;
		}
		if (e->has<MeshComponent>())
		{
			mask[ComponentTypeToString(ComponentType::MeshComponent)] = true;
		}
		if (e->has<AnimationComponent>())
		{
			mask[ComponentTypeToString(ComponentType::AnimationComponent)] = true;
		}
		ar(cereal::make_nvp("mask", mask));

		// ----------------
		for (auto& [type, exist] : mask)
		{
			if (type == "CameraComponent" && exist)
				ar(cereal::make_nvp("CameraComponent", *e->component<CameraComponent>()));
			if (type == "PointLightComponent" && exist)
				ar(cereal::make_nvp("PointLightComponent", *e->component<PointLightComponent>()));
			if (type == "DirectionalLightComponent" && exist)
				ar(cereal::make_nvp("DirectionalLightComponent", *e->component<DirectionalLightComponent>()));
			if (type == "TransformComponent" && exist)
				ar(cereal::make_nvp("TransformComponent", *e->component<TransformComponent>()));
			if (type == "MeshComponent" && exist)
				ar(cereal::make_nvp("MeshComponent", *e->component<MeshComponent>()));
			if (type == "AnimationComponent"&& exist)
				ar(cereal::make_nvp("AnimationComponent", *e->component<AnimationComponent>()));
		}

		if (e->has<MeshComponent>())
		{
			const auto& assetData = ResourceManager::getInstance()->m_AssetDataMap.at(e->component<MeshComponent>()->assetName);
			
			std::string path = Config::k_WorkingProjectRoot + e->component<MeshComponent>()->assetPath;
			std::ofstream outAsset(path);
			cereal::JSONOutputArchive ar(outAsset);
			ar(
				cereal::make_nvp("assetID", assetData.assetID),
				cereal::make_nvp("assetName", assetData.assetName),
				cereal::make_nvp("assetPath", e->component<MeshComponent>()->assetPath),
				cereal::make_nvp("meshURI", assetData.meshURI),
				cereal::make_nvp("materials", assetData.materials),
				cereal::make_nvp("textures", assetData.textures)
			);
		}

		return true;
	}

	bool EntitySerializer::LoadEntity(std::shared_ptr<Scene>& scene, const std::string& filePath)
	{
		fs::path loadPath;
		// if (filePath.length() > 0)
		// {
		// 	loadPath = filePath;
		// }
		// else
		// {
		// 	// FileSystem::SPWCreateDirectory(Config::k_WorkingProjectAssets + "Prefabs/");
		// 	// std::string savename = "prefab";
		// 	// savePath = Config::k_WorkingProjectAssets + "Prefabs/" + savename + ".json";
		// 	// while (std::filesystem::exists(savePath))
		// 	{
		// 		/// savename += "(1)";
		// 	}
		// 	loadPath = Config::k_WorkingProjectAssets + "Prefabs/" + "prefab.prefab";
		// }

		if (!fs::exists(loadPath))
		{
			std::cerr << "file does not exist !\n";
			return false;
		}
		
		std::ifstream inPrefab(loadPath);
		cereal::JSONInputArchive ar(inPrefab);
		
		// auto idComponent = IDComponent{};
		
		// CameraComponent camera_component{};
		// PointLightComponent pLight_component{};
		// DirectionalLightComponent dLight_component{};
		// TransformComponent transform_component{};
		// MeshComponent mesh_component{};
		// AnimationComponent anim_component{};

		EntityNode node{};
		ar(cereal::make_nvp("EntityNode", node));
		auto e = scene->createEntity(node.name, UUID(node.uuid));

		std::unordered_map<std::string, bool> mask;
		ar(cereal::make_nvp("mask", mask));


		for (auto& [type, exist] : mask)
		{
			if (type == "CameraComponent" && exist)
			{
				auto com = CameraComponent{};
				ar(cereal::make_nvp("CameraComponent", com));
				e->emplace<CameraComponent>(com);
			}
			if (type == "PointLightComponent" && exist)
			{
				auto com = PointLightComponent{};
				ar(cereal::make_nvp("PointLightComponent", com));
				e->emplace<PointLightComponent>(com);
			}
			if (type == "DirectionalLightComponent" && exist)
			{
				auto com = *e->emplace<DirectionalLightComponent>();
				ar(cereal::make_nvp("DirectionalLightComponent", com));
			}
			if (type == "TransformComponent" && exist)
			{
				auto com = TransformComponent{};
				ar(cereal::make_nvp("TransformComponent", com));
				e->emplace<TransformComponent>(com);
			}
			if (type == "MeshComponent" && exist)
			{
				auto com = MeshComponent{};
				ar(cereal::make_nvp("MeshComponent", com));
				e->emplace<MeshComponent>(com);
			}
			if (type == "AnimationComponent" && exist)
			{
				auto com = AnimationComponent{};
				ar(cereal::make_nvp("AnimationComponent", com));
				e->emplace<AnimationComponent>(com);
			}
		}

		const auto& rm = ResourceManager::getInstance();
		auto& mesh = *e->component<MeshComponent>();
		mesh.bindCamera = (rm->activeCameraID);
		mesh.ready = false;

		std::cout << mesh.assetName << std::endl;

		{
			mesh.bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
			mesh.modelSubPassPrograms[GET_RENDER_NODE(SPW::kPointShadowNode)] = GET_SHADER_DESC(SPW::kPointShadowShader).uuid;
			mesh.modelSubPassPrograms[GET_RENDER_NODE(SPW::kDirectionalShadowNode)] = GET_SHADER_DESC(SPW::kDirectionalShadowShader).uuid;
			mesh.modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(SPW::kGBufferShader).uuid;
		}

		return true;
	}

	bool EntitySerializer::SaveScene(const std::shared_ptr<Scene>& scene, const std::string& filePath,
	                                 const std::string& username)
	{
		fs::path savePath;
		if (filePath.length() > 0)
		{
			savePath = filePath;
		}
		else
		{
			if(username.empty())
			{
				savePath = Config::k_WorkingProjectScenes + "/" + FileSystem::GetSystemTime() + Extension::SceneEx;
			}
			else 
				savePath = Config::k_WorkingProjectScenes + "/"+ username + Extension::SceneEx;
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

				// save asset data at the same time
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

		return true;
	}

	bool EntitySerializer::LoadScene(const std::shared_ptr<Scene>& scene, const std::string& path)
	{
		// Read from .json || .scene ( both in .json file )
		std::string pathstr = path;
		FileSystem::ResolveSlash(pathstr); 

		std::ifstream inputScene(pathstr); // TODO: Select the asset file to Load by GUI operations
		cereal::JSONInputArchive ar(inputScene);

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

					mesh->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
					mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kPointShadowNode)] = GET_SHADER_DESC(SPW::kPointShadowShader).uuid;
					mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kDirectionalShadowNode)] = GET_SHADER_DESC(SPW::kDirectionalShadowShader).uuid;
					mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(SPW::kGBufferShader).uuid;
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
} // namespace SPW
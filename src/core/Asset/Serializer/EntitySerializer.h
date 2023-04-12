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
#include "IO/FileSystem.h"

namespace SPW
{
    struct EntityNode
    {
        std::string uuid;
        std::string name;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar( cereal::make_nvp("uuid", uuid), cereal::make_nvp("name", name) );
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
        	if( filePath.length() > 0 )
            {
                savePath = filePath;
            }
			else
			{
                savePath = FileRoots::k_Scenes + "/scene.json";
			}

        	std::unordered_map<std::string, CameraComponent> cameraComponents;
            std::unordered_map<std::string, PointLightComponent> pointLightComponents;
            std::unordered_map<std::string, DirectionalLightComponent> directionalLightComponents;
            std::unordered_map<std::string, TransformComponent> transformComponents;
            std::unordered_map<std::string, MeshComponent> meshComponents;
            std::vector<EntityNode> entityNodes;

        	scene->forEachEntity<IDComponent>([&](const Entity& e)
            {
                const std::string uuid_str = e.component<IDComponent>()->getID().toString();

                entityNodes.emplace_back( EntityNode{uuid_str, e.component<NameComponent>()->getName()} );

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

            });

            std::ofstream of_file(savePath);
            cereal::JSONOutputArchive ar(of_file);
            ar(cereal::make_nvp("entityNodes", entityNodes));
            ar(cereal::make_nvp("cameraComponents", cameraComponents));
            ar(cereal::make_nvp("pointLightComponents", pointLightComponents));
            ar(cereal::make_nvp("directionalComponents", directionalLightComponents));
            ar(cereal::make_nvp("transformComponents", transformComponents));
            ar(cereal::make_nvp("meshComponents", meshComponents));

            for(auto&[k, v] : meshComponents)
        	{
                if(v.b_Asset)
                {
                    { /* Save Asset Json */
                        std::ofstream file(FileSystem::ToAbsolutePath(v.assetPath));
                        cereal::JSONOutputArchive ar(file);
                        ar(
                            cereal::make_nvp("assetID", v.assetID),
                            cereal::make_nvp("assetPath", v.assetPath),
                            cereal::make_nvp("meshURI", v.meshURI),
                            cereal::make_nvp("materials", v.materials),
                            cereal::make_nvp("textures", v.textures)
                        );

                    }

                    { /* Save Mesh Bin */
                        std::string dir = FileSystem::ToFsPath(v.assetPath).parent_path().string();
                    	std::ofstream mesh_bin(FileSystem::ToAbsolutePath(FileSystem::JoinPaths(dir, v.meshURI) + ".mesh"), std::ios::binary);
                        cereal::BinaryOutputArchive archive(mesh_bin);
                        archive(cereal::make_nvp(v.meshURI, v.meshes));
                    }
                }
                else {} // TODO: Call Save Asset 's logic to save a .asset file.
            }

            return true;
        }
    };

} // namespace SPW

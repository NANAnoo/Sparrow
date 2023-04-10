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


        // void AddComponment(std::string name, toml::table componentTable)
        // {
        //     m_ComponentRecords.emplace(name, componentTable);
        // }

        static void SaveScene(const std::shared_ptr<Scene>& scene, const std::string& filePath)
        {
            std::unordered_map<std::string, CameraComponent> cameraComponents;
            std::unordered_map<std::string, PointLightComponent> pointLightComponents;
            std::unordered_map<std::string, DirectionalLightComponent> directionalLightComponents;
            std::unordered_map<std::string, TransformComponent> transformComponents;
            std::unordered_map<std::string, MeshComponent> meshComponents;
            std::vector<EntityNode> entityNodes;

        	scene->forEachEntity<IDComponent>([&](const SPW::Entity& e)
            {
                // EntitySerializer entitySerialer;

                const std::string uuid_str = e.component<SPW::IDComponent>()->getID().toString();
                // Get name and uuid from each entity.
                // std::string entity_name = e.component<SPW::NameComponent>()->getName();

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

            std::cout << FileRoots::k_Scenes + "/test__save__scene.json\n";
            std::ofstream of_file(FileRoots::k_Scenes + "/test__save__scene.json");
            cereal::JSONOutputArchive ar(of_file);
            ar(cereal::make_nvp("entityNodes", entityNodes));
            ar(cereal::make_nvp("cameraComponents", cameraComponents));
            ar(cereal::make_nvp("pointLightComponents", pointLightComponents));
            ar(cereal::make_nvp("directionalComponents", directionalLightComponents));
            ar(cereal::make_nvp("transformComponents", transformComponents));
            ar(cereal::make_nvp("meshComponents", meshComponents));

            for(auto&[k, v] : meshComponents)
        	{
                // v.meshURI = k;
                if(v.b_Asset)
                {
                    std::ofstream of_file2(v.assetPath);
                    cereal::JSONOutputArchive ar2(of_file2);
                    ar2(
                        // cereal::make_nvp("assetType", model_0->type),
                        cereal::make_nvp("assetID", v.assetID),
                        cereal::make_nvp("assetPath", v.assetPath),
                        // cereal::make_nvp("assetName", model_0->name),
                        cereal::make_nvp("meshURI", v.meshURI),
                        cereal::make_nvp("materials", v.materials),
                        cereal::make_nvp("textures", v.textures)
                    );

                    {
                        std::string dir = FileSystem::ToFsPath(v.assetPath).parent_path().string();
                    	std::ofstream mesh_bin(std::string(dir + v.meshURI + ".mesh"), std::ios::binary);
                        cereal::BinaryOutputArchive archive(mesh_bin);
                        archive(cereal::make_nvp(v.meshURI, v.meshes));
                    }
                }
     //            else TODO
     //            {
					// // 重新调用一遍SaveAsset的逻辑， 存一份.asset.
     //                std::ofstream of_file2(filePath + k + ".asset");
     //                cereal::JSONOutputArchive ar2(of_file2);
     //                ar2(
     //                    // cereal::make_nvp("meshURI", k),
     //                    cereal::make_nvp("materials", v.materials),
     //                    cereal::make_nvp("textures", v.textures)
     //                );
     //
     //                {
     //                    std::ofstream mesh_bin(std::string(filePath + k + ".mesh"), std::ios::binary);
     //                    cereal::BinaryOutputArchive archive(mesh_bin);
     //                    archive(cereal::make_nvp(k, v.meshes));
     //                }
     //            }


            }
        }


        // // TODO: should directly return a scene tree.
        // std::vector<std::shared_ptr<SPW::Entity>> LoadScene(std::shared_ptr<Scene> scene, const std::string& filePath)
        // {
        //     toml::table tbl{};
        //     try
        //     {
        //         tbl = toml::parse_file(filePath);
        //         std::cout << tbl << "\n";
        //     }
        //     catch (const toml::parse_error& err)
        //     {
        //         std::cerr << "Parsing failed:\n" << err << "\n";
        //     }
        //
        //     std::vector<std::shared_ptr<SPW::Entity>> scene_entities;
        //     for (const auto& [k, v] : tbl)
        //     {
        //         // Create an entity:
        //         const char* uuid_str = k.data();
        //         auto ent = scene->createEntityByID(UUID{ uuid_str });
        //
        //         std::cout << "entity:" << k << std::endl;
        //
        //         for (const auto& [ik, iv] : *v.as<toml::table>())
        //         {
        //             const auto& ik_name = std::string(ik);
        //
        //             if (ik_name == "name")
        //             {
        //                 auto ent_name = std::string(*iv.as<std::string>());
        //                 std::cout << "name:" << ent_name << std::endl;
        //                 ent->emplace<SPW::NameComponent>(ent_name);
        //             }
        //
        //             if (ik_name == "components")
        //             {
        //                 // std::cout << "components:" << *iv.as<std::string>() << std::endl;
        //                 for (const auto& [ck, cv] : *iv.as<toml::table>())
        //                 {
        //                     // std::cout << "component name:" << ck << std::endl;
        //
        //                     if (ck == "light")
        //                     {
        //                         // std::cout << "Init Light Component" << std::endl;
        //                         auto component_table = *cv.as<toml::table>();
        //
        //                         LightComponent tmp_light_component{ LightType::PointLightType };
        //                         tmp_light_component.load(std::move(component_table));
        //
        //                         ent->emplace<SPW::LightComponent>(tmp_light_component);
        //                     }
        //                     if (ck == "camera")
        //                     {
        //                         auto component_table = *cv.as<toml::table>();
        //
        //                         CameraComponent tmp_camera_component{ CameraType::PerspectiveType };
        //                         tmp_camera_component.load(std::move(component_table));
        //
        //                         ent->emplace<SPW::CameraComponent>(tmp_camera_component);
        //                     }
        //                 }
        //             }
        //         }
        //
        //         scene_entities.emplace_back(ent);
        //     }
        //
        //     return scene_entities;
        // }
        //
        // void SaveEntity(std::string com_name, std::string& UUID)
        // {
        //     toml::table componet_table;
        //     for (const auto& [key, value] : m_ComponentRecords)
        //         componet_table.insert_or_assign(key, value);
        //
        //     auto tbl = toml::table
        //     {
        //         {UUID, toml::table
        //             {
        //                 {"name", com_name},
        //                 {"components",componet_table},
        //             }},
        //     };
        //
        //     return tbl;
        // }
        //
        // std::unordered_map<std::string, SaveTable> m_ComponentRecords;
    };

} // namespace SPW

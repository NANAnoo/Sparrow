#include "EntitySerializer.h"
#include "EcsFramework/Component/LightComponent.hpp"
#include "EcsFramework/Component/CameraComponent.hpp"

namespace SPW
{

std::vector<std::shared_ptr<SPW::Entity>> EntitySerializer::LoadScene(std::shared_ptr<Scene> scene, const std::string& filePath)
{
    toml::table tbl{};
    try
    {
      tbl = toml::parse_file(filePath);
      std::cout << tbl << "\n";
    }
    catch (const toml::parse_error& err)
    {
      std::cerr << "Parsing failed:\n" << err << "\n";
    }

    std::vector<std::shared_ptr<SPW::Entity>> scene_entities;
    for(const auto&[k,v] : tbl)
    {
      // Create an entity:
      const char* uuid_str = k.data();
      auto ent = scene->createEntityByID(UUID{uuid_str});

      std::cout << "entity:" << k << std::endl;

      for (const auto& [ik, iv] : *v.as<toml::table>())
      {
        const auto& ik_name = std::string(ik);

        if(ik_name == "name")
        {
          auto ent_name = std::string(*iv.as<std::string>());
          std::cout << "name:" << ent_name << std::endl;
          ent->emplace<SPW::NameComponent>(ent_name);
        }

        if(ik_name == "components")
        {
          // std::cout << "components:" << *iv.as<std::string>() << std::endl;
          for (const auto& [ck, cv] : *iv.as<toml::table>())
          {
            // std::cout << "component name:" << ck << std::endl;

            if(ck == "light")
            {
              // std::cout << "Init Light Component" << std::endl;
              auto component_table = *cv.as<toml::table>();

              LightComponent tmp_light_component{LightType::PointLightType};
              tmp_light_component.load(std::move(component_table));

              ent->emplace<SPW::LightComponent>(tmp_light_component);
            }
            if(ck == "camera")
            {
              auto component_table = *cv.as<toml::table>();

              CameraComponent tmp_camera_component{CameraType::PerspectiveType};
              tmp_camera_component.load(std::move(component_table));

              ent->emplace<SPW::CameraComponent>(tmp_camera_component);
            }
          }
        }
      }

      scene_entities.emplace_back(ent);
    }

    return scene_entities;
}

toml::table EntitySerializer::SaveEntity(std::string com_name, std::string &UUID)
{
    toml::table componet_table;
    for (const auto& [key, value] : m_ComponentRecords)
        componet_table.insert_or_assign(key, value);

    auto tbl = toml::table
    {
        {UUID, toml::table
            {
                {"name", com_name},
                {"components",componet_table},
            }},
    };

    return tbl;
}

void EntitySerializer::SaveScene(std::shared_ptr<Scene> scene, const std::string& filePath)
{
    std::vector<toml::table> sceneSerialer;
    scene->forEachEntity<SPW::IDComponent>([&sceneSerialer](const SPW::Entity &e)
    {
        SPW::EntitySerializer entitySerialer;
        // TODO: Iterate all components and save them.
        if(e.has<SPW::CameraComponent>())
          entitySerialer.AddComponment("camera", e.component<SPW::CameraComponent>()->save());
        if(e.has<SPW::LightComponent>())
          entitySerialer.AddComponment("light", e.component<SPW::LightComponent>()->save());

        // Get name and uuid from each entity.
        auto com_name = e.component<SPW::NameComponent>()->getName();
        auto uuid_str = e.component<SPW::IDComponent>()->getID().toString();

        // std::cout << entitySerialer.SaveEntities(com_name, uuid_str) << std::endl;
        sceneSerialer.emplace_back( entitySerialer.SaveEntity(com_name, uuid_str) );
    });

    // Write toml To File
    std::ofstream file(filePath);
    if (file.is_open())
    {
        for(const auto e: sceneSerialer)
        {
          file << e << std::endl << std::endl;
        }
        file.close();
    }
}


} // namespace SPW
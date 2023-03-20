#pragma once

#include <toml++/toml.h>
#include <array>
#include <unordered_map>
#include "SerializerUtil.inl"
#include "EcsFramework/Scene.hpp"

namespace SPW 
{
  class EntitySerializer
  {
  public:
      EntitySerializer() = default;
      ~EntitySerializer() = default;
      EntitySerializer(const EntitySerializer & ts) = delete;
      EntitySerializer(EntitySerializer && ts) = delete;

      void AddComponment(std::string name, toml::table componentTable)
      {
          m_ComponentRecords.emplace(name, componentTable);
      }

      void SaveScene(std::shared_ptr<Scene> scene, const std::string& filePath);

      // TODO: should directly return a scene tree.
      std::vector<std::shared_ptr<SPW::Entity>> LoadScene(std::shared_ptr<Scene> scene, const std::string& filePath);

  public:
      template <typename T> inline static toml::array ToArray(const T& vec){ return sut::to_array(vec); }
      inline static glm::vec3 ToVec3(toml::array&& vec){ return sut::toVec3(std::forward<toml::array>(vec)); }

  private:
      toml::table SaveEntity(std::string com_name, std::string& UUID);

  private:
      std::unordered_map<std::string, SaveTable> m_ComponentRecords;
  };
} // namespace SPW

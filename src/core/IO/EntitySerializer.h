#pragma once

#include <toml++/toml.h>
#include <array>
#include <unordered_map>
#include "SerializerUtil.inl"

namespace SPW 
{

  using SaveTable = toml::table;

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

    SaveTable SaveEntity(std::string com_name, std::string& UUID)
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

  public:
    template <typename T> inline static toml::array ToArray(const T& vec){ return sut::to_array(vec); }

  private:
    std::unordered_map<std::string, SaveTable> m_ComponentRecords;
  };

} // namespace SPW

//
// Created by dudu on 2023/3/10.
//

#ifndef SPARROW_TOMLSERIALIZER_H
#define SPARROW_TOMLSERIALIZER_H
#include <toml++/toml.h>

namespace SPW {

  class TomlSerializer
  {
      toml::table GetTable()
    {
        toml::array arr{1.0f, 2.0f, 3.0f};

        auto light_table = toml::table
            {
                { "lit_position", arr },
                { "lit_direction", arr },
                { "lit_type", "three" }
            };

        auto tranform = toml::table
            {
                { "pos", arr },
                { "direction", arr },
                { "rotation", "three" }
            };

        auto tbl = toml::table
            {
                { "light", light_table},
                { "v", toml::table{
                          { "name", "Mark Gillard" },
                          { "transform", tranform},
                          { "twitter", "https://twitter.com/marzer8789" }
                      }
                },
            };

        return tbl;
      }
  };

} // namespace SPW

#endif // SPARROW_TOMLSERIALIZER_H

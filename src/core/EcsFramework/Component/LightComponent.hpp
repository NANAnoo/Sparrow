//
// Created by ZhangHao on 2023/2/20.
//
#pragma once
#include "ComponentI.h"
#include "IO/EntitySerializer.h"

#include <glm/glm.hpp>

namespace SPW {
    // light types
    enum LightType {
        PointLightType,
        DirectionalLightType
    };

    // store light info
    // 1. type
    // 2. intensity
    // 3. color
    class LightComponent : public ComponentI {
    public:
        LightComponent() = delete;
        explicit LightComponent(LightType type) : lightType(type) { }

        float constant = 1;
        float linear = 1;
        float quadratic = 1;  

        glm::vec3 ambient = {1, 1, 1};
        glm::vec3 diffuse = {1, 1, 1};
        glm::vec3 specular = {1, 1, 1};

        [[nodiscard]] LightType getType() const {
            return lightType;
        };

        SaveTable save()
        {
            auto tbl = SaveTable
                {
                    {"constant", constant},
                    {"linear", linear},
                    {"quadratic", quadratic},
                    {"ambient", EntitySerializer::ToArray(ambient)},
                    {"diffuse", EntitySerializer::ToArray(diffuse)},
                    {"specular", EntitySerializer::ToArray(specular)},
                    {"LightType", static_cast<int>(getType())},
                };
            return tbl;
        }

        LightComponent& load(SaveTable&& tbl)
        {
            LightComponent light_tmp{LightType::DirectionalLightType};
            for (const auto& [k, v] : tbl)
            {
              std::cout << k << std::endl;
              const auto& flied_name = std::string(k);
              if (flied_name == "LightType")
              {
                light_tmp.lightType = static_cast<LightType>(int64_t(*v.as_integer()));
              }
              if (flied_name == "ambient")
              {
                auto arr = *v.as_array();
                light_tmp.ambient = EntitySerializer::ToVec3(std::move(arr));
              }
              if (flied_name == "constant")
              {
                light_tmp.constant = static_cast<float>(double(*v.as_floating_point()));
              }
              if (flied_name == "diffuse")
              {
                auto arr = *v.as_array();
                light_tmp.diffuse = EntitySerializer::ToVec3(std::move(arr));
              }
              if (flied_name == "linear")
              {
                light_tmp.linear = static_cast<float>(double(*v.as_floating_point()));
              }
              if (flied_name == "quadratic")
              {
                light_tmp.quadratic = static_cast<float>(double(*v.as_floating_point()));

              }
              if (flied_name == "specular")
              {
                auto arr = *v.as_array();
                light_tmp.specular = EntitySerializer::ToVec3(std::move(arr));
              }
            }

            return light_tmp;
        }


    private:
        LightType lightType;
    };
}
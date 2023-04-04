#pragma once
#include "../ComponentI.h"
#include <glm/glm.hpp>

namespace SPW {
    class PointLightComponent : public ComponentI {
    public:
        PointLightComponent() = default;

        float constant;
        float linear;
        float quadratic;  

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        // lua binding
        void update(const std::string &key, const sol::table &value) final {
            if (key == "constant" && value["value"].valid()) {
                constant = value["value"];
            }
            if (key == "linear" && value["value"].valid()) {
                linear = value["value"];
            }
            if (key == "quadratic" && value["value"].valid()) {
                quadratic = value["value"];
            }
            if (key == "ambient" && value["value"].valid()) {
                ambient = value["value"];
            }
            if (key == "diffuse" && value["value"].valid()) {
                diffuse = value["value"];
            }
            if (key == "specular" && value["value"].valid()) {
                specular = value["value"];
            }
        }

        // init from lua
        void initFromLua(const sol::table &value) {
            if (value["constant"].valid()) {
                constant = value["constant"];
            }
            if (value["linear"].valid()) {
                linear = value["linear"];
            }
            if (value["quadratic"].valid()) {
                quadratic = value["quadratic"];
            }
            if (value["ambient"].valid()) {
                ambient = value["ambient"];
            }
            if (value["diffuse"].valid()) {
                diffuse = value["diffuse"];
            }
            if (value["specular"].valid()) {
                specular = value["specular"];
            }
        }

        // getLuaValue
        virtual sol::object getLuaValue(const sol::table &value, const std::string &key) final {
            if (key == "constant") {
                return sol::make_object(value.lua_state(), constant);
            }
            if (key == "linear") {
                return sol::make_object(value.lua_state(), linear);
            }
            if (key == "quadratic") {
                return sol::make_object(value.lua_state(), quadratic);
            }
            if (key == "ambient") {
                return sol::make_object(value.lua_state(), ambient);
            }
            if (key == "diffuse") {
                return sol::make_object(value.lua_state(), diffuse);
            }
            if (key == "specular") {
                return sol::make_object(value.lua_state(), specular);
            }
            return sol::nil;
        }
    };
}

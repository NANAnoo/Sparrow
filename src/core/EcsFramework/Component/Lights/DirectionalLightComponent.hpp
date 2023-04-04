#pragma once
#include "../ComponentI.h"
#include <glm/glm.hpp>

namespace SPW {
    class DirectionalLightComponent : public ComponentI {
    public:
        DirectionalLightComponent() = default;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        // lua binding
        void update(const std::string &key, const sol::table &value) final {
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
//
// Created by ZhangHao on 2023/2/20.
//
#pragma once

#include "ComponentI.h"
#include <glm/glm.hpp>

namespace SPW {
    class TransformComponent : public ComponentI {
    public:
        TransformComponent() = default;
        TransformComponent(const TransformComponent &other) {
            position = other.position;
            rotation = other.rotation;
            scale = other.scale;
        };

        glm::vec3 position = {0, 0, 0};
        glm::vec3 rotation = {0, 0, 0};
        glm::vec3 scale = {1, 1, 1};

        // update from lua
        void update(const std::string &key, const sol::table &value) final {
            if (key == "position" && value["value"].valid()) {
                position = value["value"];
            }
            if (key == "rotation" && value["value"].valid()) {
                rotation = value["value"];
            }
            if (key == "scale" &&value["value"].valid()) {
                scale = value["value"];
            }
        }

        void initFromLua(const sol::table &value) {
            if (value["position"].valid()) {
                position = value["position"];
            }
            if (value["rotation"].valid()) {
                rotation = value["rotation"];
            }
            if (value["scale"].valid()) {
                scale = value["scale"];
            }
        }

        // getLuaValue
        virtual sol::object getLuaValue(const sol::table &value, const std::string &key) final {
            if (key == "position") {
                return sol::make_object(value.lua_state(), position);
            }
            if (key == "rotation") {
                return sol::make_object(value.lua_state(), rotation);
            }
            if (key == "scale") {
                return sol::make_object(value.lua_state(), scale);
            }
            return sol::nil;
        }
    };
}

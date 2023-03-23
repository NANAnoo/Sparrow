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
        void update(const sol::table &value) final {
            if (value["position"].valid()) {
                position = value["position"];
            }
            if (value["rotation"].valid()) {
                rotation = value["rotation"];
                std::printf("current x:%.3f \n", rotation.x);
            }
            if (value["scale"].valid()) {
                scale = value["scale"];
            }
        }
    };
}

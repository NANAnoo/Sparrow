//
// Created by ZhangHao on 2023/2/20.
//
#pragma once
#include "ComponentI.h"

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
        explicit LightComponent(LightType type, float intensity, glm::vec3 color) :
            lightType(type), lightIntensity(intensity), lightColor(color) {

        }
        float lightIntensity = 1.f;
        glm::vec3 lightColor = {0, 0, 0};
        [[nodiscard]] LightType getType() const {
            return lightType;
        };
    private:
        LightType lightType;
    };
}
#pragma once

#include "Render/RenderGraph.hpp"

namespace SPW {
    static ShaderDesc FXAA_desc(const AttachmentPort &port) {
        ShaderDesc res{};
        res.shader = ShaderHandle(
            "FXAA", 
            "./resources/shaders/screen.vert",
            "./resources/shaders/FXAA.frag");
        res.dependency_inputs[port] = "screenTexture";
        return res;
    }

    static ShaderDesc P_shadowmap_desc() {
        ShaderDesc res{};

        res.transform_inputs[TransformType::M] = "M";
        res.transform_inputs[TransformType::VP] = "VP";
        res.light_inputs[LightType::PointLight] = "light";

        res.shader = {
            "P_shadow",
            "./resources/shaders/P_shadowMap.vert",
            "./resources/shaders/P_shadowMap.frag"
        };

        return res;
    }

    static ShaderDesc D_shadowmap_desc() {
        ShaderDesc res{};

        res.transform_inputs[TransformType::M] = "M";
        res.light_inputs[LightType::DLightTransform] = "lightSpaceMatrix";

        res.shader = {
            "D_shadow",
            "./resources/shaders/shadowMap.vert",
            "./resources/shaders/shadowMap.frag"
        };

        return res;
    }

    static ShaderDesc PBR_light_with_shadow_desc(const AttachmentPort &p_shaodw, const AttachmentPort &d_shaodw, const ShaderHandle &shader) {
        ShaderDesc res{};

        res.mat_inputs[MaterialType::AlbedoType] = "albedoMap";
        res.mat_inputs[MaterialType::NormalType] = "normalMap";
        res.mat_inputs[MaterialType::MetallicType] = "metallicMap";
        res.mat_inputs[MaterialType::RoughnessType] = "roughnessMap";
        res.mat_inputs[MaterialType::AOType] = "AoMap";

        res.context_inputs[ContextType::CameraPosition] = "camPos";
        res.context_inputs[ContextType::RandomNumber] = "RandomSeed";

        res.light_inputs[LightType::PointLight] = "PLights";
        res.light_inputs[LightType::DirectionalLight] = "DLights";
        res.light_inputs[LightType::DLightArraySize] = "DLightCount";
        res.light_inputs[LightType::PLightArraySize] = "PLightCount";
        res.light_inputs[LightType::DLightTransform] = "lightSpaceMatrix";

        res.dependency_inputs[p_shaodw] = "P_shadowMap";
        res.dependency_inputs[d_shaodw] = "shadowMap";

        res.transform_inputs[TransformType::M] = "M";
        res.transform_inputs[TransformType::V] = "V";
        res.transform_inputs[TransformType::P] = "P";

        res.shader = shader;

        return res;
    }

}



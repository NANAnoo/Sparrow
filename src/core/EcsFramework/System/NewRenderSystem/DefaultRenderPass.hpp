#pragma once

#include "Render/RenderGraph.hpp"

namespace SPW {
    static const ModelSubPassConfig D_shadowPass() {
        ModelSubPassConfig config = {};

        config.frameWidth = 4096;
        config.frameHeight = 4096;

        config.depthCompType = DepthCompType::LESS_Type;
        config.depthTest = true;

        config.repeatType = RepeatType::RepeatForDLights;
        config.repeatCount = 10;

        config.transform_inputs[TransformType::M] = "M";
        config.light_inputs[LightType::DLightTransform] = "lightSpaceMatrix";

        config.outputType = AttachmentType::ColorType;
        config.attachments.push_back(ColorAttachmentFormat::Depth);

        return config;
    }

    static const ModelSubPassConfig P_shadowPass() {
        ModelSubPassConfig config = {};

        config.frameWidth = 1000;
        config.frameHeight = 1000;

        config.depthCompType = DepthCompType::LESS_Type;
        config.depthTest = true;

        config.repeatType = RepeatType::RepeatForPLights;
        config.repeatCount = 10;

        config.transform_inputs[TransformType::M] = "M";
        config.transform_inputs[TransformType::V] = "V";
        config.transform_inputs[TransformType::P] = "P";
        config.light_inputs[LightType::PointLight] = "light";

        config.outputType = AttachmentType::CubeMapType;
        config.attachments.push_back(ColorAttachmentFormat::Depth);

        return config;
    }

    static const ModelSubPassConfig F_pbr_lightPass() {
        ModelSubPassConfig config = {};

        config.frameWidth = 0;
        config.frameHeight = 0;

        config.depthCompType = DepthCompType::LESS_Type;
        config.depthTest = true;

        config.repeatType = RepeatType::NoRepeat;
        config.repeatCount = 1;

        config.mat_inputs[MaterialType::AlbedoType] = "albedoMap";
        config.mat_inputs[MaterialType::NormalType] = "normalMap";
        config.mat_inputs[MaterialType::MetallicType] = "metallicMap";
        config.mat_inputs[MaterialType::RoughnessType] = "roughnessMap";
        config.mat_inputs[MaterialType::AOType] = "AoMap";

        config.context_inputs[ContextType::CameraPosition] = "camPos";
        config.context_inputs[ContextType::RandomNumber] = "RandomSeed";

        config.light_inputs[LightType::PointLight] = "PLights";
        config.light_inputs[LightType::DirectionalLight] = "DLights";
        config.light_inputs[LightType::DLightArraySize] = "DLightCount";
        config.light_inputs[LightType::PLightArraySize] = "PLightCount";
        config.light_inputs[LightType::DLightTransform] = "lightSpaceMatrix";

        config.previous_attachment_inputs[0] = "shadowMap";

        config.transform_inputs[TransformType::M] = "M";
        config.transform_inputs[TransformType::V] = "V";
        config.transform_inputs[TransformType::P] = "P";

        config.outputType = AttachmentType::UnknownAttachmentType;

        config.screenOutputType = ScreenColorType;

        return config;
    }

    static const ImageSubPassConfig FXAA_postPass () {
        ImageSubPassConfig config = {};

        config.frameHeight = 0;
        config.frameWidth = 0;

        config.screen_attachment_inputs[ScreenColorType] = "screenTexture";
        config.outputType = UnknownAttachmentType;

        config.screenOutputType = PresentType;

        config.shader = ShaderHandle(
            "FXAA", 
            "./resources/shaders/screen.vert",
            "./resources/shaders/FXAA.frag");
        
        return config;
    };

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



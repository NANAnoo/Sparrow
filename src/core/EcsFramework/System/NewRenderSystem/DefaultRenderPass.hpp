#pragma once

#include "Render/PipeLine.hpp"

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
}



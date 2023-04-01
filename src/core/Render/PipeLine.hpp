#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <glm/glm.hpp>

#include "Render/Shader.h"

namespace SPW {
    enum DepthCompType
    {
        EQUAL_Type,
        LEQUAL_Type,
        LESS_Type
    };
    enum MaterialType {
        // textures from files
        AlbedoType = 1 << 1,
        NormalType = 1 << 2,
        MetallicType = 1 << 3,
        RoughnessType = 1 << 4,
        AOType = 1 << 5,

        // AlbedoConstant is vec3
        AlbedoConstantType = 1 << 6,
        // NormalConstantType = 1 << 7,

        // float
        MetallicConstantType = 1 << 8,
        RoughnessConstantType = 1 << 9,
        AOConstantType = 1 << 10,

        // diffsuse, specular, ambient, and emissive as vec3
        DiffuseType = 1 << 11,
        SpecularType = 1 << 12,
        AmbientType = 1 << 13,
        EmissiveType = 1 << 14,

        // transparent and refraction as vec3
        TransparentType = 1 << 15,
        RefractionType = 1 << 16,

        // float
        EmissiveIntensityType = 1 << 17,
        SpecularPowerType = 1 << 18,
        TransparentIntensityType = 1 << 19,
        RefractionIntensityType = 1 << 20,

        // cubemap from files
        CubeTextureType = 1 << 21,
    };

    // camera uniforms
    enum TransformType {
        // camera related transform
        M = 1 << 1,
        V = 1 << 2,
        P = 1 << 3,
        MVP = 1 << 4,
        MV = 1 << 5,
        VP = 1 << 6,
    };

    // Context related uniforms
    enum ContextType {
        // frame size
        FrameWidth = 1 << 1,
        FrameHeight = 1 << 2,
        // camera position
        CameraPosition = 1 << 3,
        // random uniform float
        RandomNumber = 1 << 4,
    };

    // light uniforms, should be uniform array object
    enum LightType {
        // light input point light array
        PointLight = 1 << 1,
        // light input directional light array
        DirectionalLight = 1 << 2,
        // dirential light transform
        DLightTransform = 1 << 3,
        // plight array size
        PLightArraySize = 1 << 4,
        // dlight array size
        DLightArraySize = 1 << 5,
    };

    enum ClearType {
        // don't clear
        NoClear = 0,
        ClearColor = 1 << 1,
        ClearDepth = 1 << 2,
        ClearStencil = 1 << 3,
    };

    // input frame buffer type, get from previous pipeline output
    enum AttachmentType {
        UnknownAttachmentType = 0,
        // color
        ColorType = 1 << 1,

        // cube map
        CubeMapType = 1 << 2,

        // save to disk, for example, for light baking 
        // SaveToDiskType = 1 << 3,
    };

    // screen space output type
    enum ScreenAttachmentType {
        // unknown type
        UnknownScreenAttachmentType = 0,
        // screen buffer
        ScreenColorType = 1 << 1,

        // screen depth buffer
        ScreenDepthType = 1 << 2,

        // screen all types
        ScreenAllType = ScreenColorType | ScreenDepthType,

        // present to screen
        PresentType = 1 << 3,
    };

    // color attachment format
    enum ColorAttachmentFormat {
        // 8 bit
        R8 = 1 << 1,
        RG8 = 1 << 2,
        RGB8 = 1 << 3,
        RGBA8 = 1 << 4,

        // 16 bit
        R16 = 1 << 5,
        RG16 = 1 << 6,
        RGB16 = 1 << 7,
        RGBA16 = 1 << 8,

        // 32 bit
        R32 = 1 << 9,
        RG32 = 1 << 10,
        RGB32 = 1 << 11,
        RGBA32 = 1 << 12,

        // depth
        Depth = 1 << 13,
    };

    // repeat info
    enum RepeatType {
        NoRepeat = 0,
        RepeatForDLights = 1 << 1,
        RepeatForPLights = 1 << 2,
        // todo : add light probe repeat type
    };

    // each model pass can be bind with to a custimized shader
    struct ModelSubPassConfig {
        // unsigned id
        unsigned int ID;

        // ClearType clearType;
        
        DepthCompType depthCompType;

        bool depthTest;

        // ModelSubPass can excute multiple times, for example, for each light
        RepeatType repeatType;
        unsigned int repeatCount;

        // 0, 0 means use the window size
        unsigned int frameWidth;
        unsigned int frameHeight;
        

        // material uniforms
        std::unordered_map<MaterialType, std::string> mat_inputs;

        // transform uniforms
        std::unordered_map<TransformType, std::string> transform_inputs;

        // context uniforms
        std::unordered_map<ContextType, std::string> context_inputs;

        // light uniforms
        std::unordered_map<LightType, std::string> light_inputs;

        // attchment textures uniforms, from previous ModelSubPass
        // previeous attachment must be in [ColorType, CubeMapType]
        std::unordered_map<unsigned int, std::string> previous_attachment_inputs;

        // MRT output
        AttachmentType outputType;
        std::vector<ColorAttachmentFormat> attachments;

        // screen output
        ScreenAttachmentType screenOutputType;
    };

    // for screen space pass, it will be binded with a fixed shader
    // can be used for deferred lighting pass , post processing, screen space effect
    struct ImageSubPassConfig {
        // unsigned id
        unsigned int ID;

        // ClearType clearType;

        // 0, 0 means use the window size
        unsigned int frameWidth;
        unsigned int frameHeight;

        // no material uniforms and transform uniforms

        // context uniforms
        std::unordered_map<ContextType, std::string> context_inputs;

        // light uniforms
        std::unordered_map<LightType, std::string> light_inputs;
        
        // attchment textures uniforms, from previous RenderSubPass
        // for the first screen pass, it will be the output of the last model pass
        // previeous attachment can'r be in [SaveToDiskType]
        std::unordered_map<unsigned int, std::string> previous_attachment_inputs;

        // screen attchment inputs
        std::unordered_map<ScreenAttachmentType, std::string> screen_attachment_inputs;

        // output, if there is no attachment, then it will be present
        // MRT support
        AttachmentType outputType;
        std::vector<ColorAttachmentFormat> attachments;

        // screen output
        ScreenAttachmentType screenOutputType;
        
        // a shader that will be used for this screen pass
        ShaderHandle shader;
    };

    struct RenderPassConfig {
        int id;
        std::vector<unsigned int> model_subpass;
        std::vector<unsigned int> image_subpass;
    };
}
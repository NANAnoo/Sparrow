#pragma once

#include "Render/RenderGraph.hpp"
#include "Asset/AssetData/Mesh.h"

namespace SPW {
    static ShaderDesc FXAA_desc(const AttachmentPort &port) {
        ShaderDesc res{};
        res.context_inputs[ContextType::FrameWidth] = "screen_width";
        res.context_inputs[ContextType::FrameHeight] = "screen_height";
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

    static ShaderDesc SSR(const AttachmentPort &gAlbedo,
                          const AttachmentPort &gMetalRognessAO,
                          const AttachmentPort &gDepth,
                          const AttachmentPort &gNormal,
                          const AttachmentPort &gPosition,
                          const AttachmentPort &ScreenColor)
    {
        ShaderDesc res{};
        res.dependency_inputs[gAlbedo] = "gAlbedo";
        res.dependency_inputs[gNormal] = "gNormal";
        res.dependency_inputs[gMetalRognessAO] = "gMetalRognessAO";
        res.dependency_inputs[gDepth] = "gDepth";
        res.dependency_inputs[ScreenColor] = "Screen";
        res.dependency_inputs[gPosition] = "gPosition";

        res.transform_inputs[TransformType::V] = "V";
        res.transform_inputs[TransformType::P] = "P";

        res.shader = {
            "SSR Shader",
            "./resources/shaders/screen.vert",
            "./resources/shaders/SSR.frag"
        };

        return res;
    }

    static ShaderDesc SSR_blur(const AttachmentPort &ssr_input,
                               const AttachmentPort &gDepth,
                               const AttachmentPort &ScreenColor)
    {
        ShaderDesc res{};

        res.dependency_inputs[ssr_input] = "reflection";
        res.dependency_inputs[gDepth] = "gDepth";
        res.dependency_inputs[ScreenColor] = "Screen";



        res.shader = {
                "SSR Shader",
                "./resources/shaders/screen.vert",
                "./resources/shaders/SSR_blur.frag"
        };

        return res;
    }


    static ShaderDesc defferPBR(const AttachmentPort &p_shaodw, const AttachmentPort &d_shaodw,
                                const AttachmentPort &gPosition,
                                const AttachmentPort &gNormal,
                                const AttachmentPort &gAlbedo,
                                const AttachmentPort &gMetalRognessAO,
                                const AttachmentPort &gDepth,
                                const ShaderHandle &shader)
    {
        ShaderDesc res{};

        res.context_inputs[ContextType::CameraPosition] = "camPos";

        res.light_inputs[LightType::PointLight] = "PLights";
        res.light_inputs[LightType::DirectionalLight] = "DLights";
        res.light_inputs[LightType::DLightArraySize] = "DLightCount";
        res.light_inputs[LightType::PLightArraySize] = "PLightCount";
        res.light_inputs[LightType::DLightTransform] = "lightSpaceMatrix";

        res.dependency_inputs[p_shaodw] = "P_shadowMap";
        res.dependency_inputs[d_shaodw] = "shadowMap";
        res.dependency_inputs[gPosition] = "gPosition";
        res.dependency_inputs[gNormal] = "gNormal";
        res.dependency_inputs[gAlbedo] = "gAlbedo";
        res.dependency_inputs[gMetalRognessAO] = "gMetalRognessAO";
        res.dependency_inputs[gDepth] = "gDepth";


        res.transform_inputs[TransformType::M] = "M";
        res.transform_inputs[TransformType::V] = "V";
        res.transform_inputs[TransformType::P] = "P";

        res.context_inputs[ContextType::RandomNumber] ="RandomSeed";

        res.shader = shader;

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

    static ShaderDesc P_ani_shadowmap_desc() {
        ShaderDesc res{};

        res.transform_inputs[TransformType::M] = "M";
        res.transform_inputs[TransformType::VP] = "VP";
        res.context_inputs[ContextType::MeshOffset] = "offset";
        res.light_inputs[LightType::PointLight] = "light";

        res.shader = {
            "P_shadow",
            "./resources/shaders/P_shadowMap_ani.vert",
            "./resources/shaders/P_shadowMap.frag"
        };

        return res;
    }

    static ShaderDesc D_ani_shadowmap_desc() {
        ShaderDesc res{};

        res.transform_inputs[TransformType::M] = "M";
        res.context_inputs[ContextType::MeshOffset] = "offset";
        res.light_inputs[LightType::DLightTransform] = "lightSpaceMatrix";

        res.shader = {
            "D_shadow",
            "./resources/shaders/shadowMap_ani.vert",
            "./resources/shaders/shadowMap.frag"
        };

        return res;
    }

    static ShaderDesc GBuffer_desc(const ShaderHandle &shader) {
        ShaderDesc res{};

        res.mat_inputs[MaterialType::AlbedoType] = "albedoMap";
        res.mat_inputs[MaterialType::NormalType] = "normalMap";
        res.mat_inputs[MaterialType::MetallicType] = "metallicMap";
        res.mat_inputs[MaterialType::RoughnessType] = "roughnessMap";
        res.mat_inputs[MaterialType::AOType] = "AoMap";

        res.transform_inputs[TransformType::M] = "M";
        res.transform_inputs[TransformType::V] = "V";
        res.transform_inputs[TransformType::P] = "P";

        res.shader = shader;

        return res;
    }

    static ShaderDesc ani_GBuffer_desc(const ShaderHandle &shader) {
        ShaderDesc res{};

        res.mat_inputs[MaterialType::AlbedoType] = "albedoMap";
        res.mat_inputs[MaterialType::NormalType] = "normalMap";
        res.mat_inputs[MaterialType::MetallicType] = "metallicMap";
        res.mat_inputs[MaterialType::RoughnessType] = "roughnessMap";
        res.mat_inputs[MaterialType::AOType] = "AoMap";

        res.transform_inputs[TransformType::M] = "M";
        res.transform_inputs[TransformType::V] = "V";
        res.transform_inputs[TransformType::P] = "P";

        res.context_inputs[ContextType::MeshOffset] = "offset";

        res.shader = shader;

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

    static ShaderDesc PBR_ani_shadow_desc(const AttachmentPort &p_shaodw, const AttachmentPort &d_shaodw, const ShaderHandle &shader) {
        ShaderDesc res{};

        res.mat_inputs[MaterialType::AlbedoType] = "albedoMap";
        res.mat_inputs[MaterialType::NormalType] = "normalMap";
        res.mat_inputs[MaterialType::MetallicType] = "metallicMap";
        res.mat_inputs[MaterialType::RoughnessType] = "roughnessMap";
        res.mat_inputs[MaterialType::AOType] = "AoMap";

        res.context_inputs[ContextType::CameraPosition] = "camPos";
        res.context_inputs[ContextType::RandomNumber] = "RandomSeed";
        res.context_inputs[ContextType::MeshOffset] = "offset";

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

    static ShaderDesc SkyBoxShader_desc() {
        ShaderDesc res{};

        res.transform_inputs[TransformType::VP] = "VP";

        res.mat_inputs[MaterialType::CubeTextureType] = "skybox";
        res.context_inputs[ContextType::CameraPosition] = "camPos";

        res.shader = {
            "SkyBox",
            "./resources/shaders/skybox.vert",
            "./resources/shaders/skybox.frag"
        };
        return res;
    }

    static Mesh createSkyBoxModel(const std::vector<std::string> &faces) {
        auto mesh = Mesh{};
        mesh.mMaterial = std::make_shared<Material>();
        mesh.mMaterial->cubeMapTextures = faces;
        // push 8 vertices on cube
        mesh.vertices.emplace_back(Vertex{glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(1.0f,  1.0f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(-1.0f,  1.0f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(1.0f,  -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(1.0f,  -1.0f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(-1.0f,  -1.0f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(-1.0f,  -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.indices = {0, 3, 4, 3, 7, 4, 3, 2, 7, 2, 6, 7, 2, 1, 6, 1, 5, 6, 1, 0, 5, 0, 4, 5, 0, 1, 2, 0, 2, 3, 4, 7, 6, 4, 6, 5};

        return mesh;
    }

	static Mesh createSkyBoxMesh() {
        auto mesh = Mesh{};
        // mesh.mMaterial = std::make_shared<Material>();
        // mesh.mMaterial->cubeMapTextures = faces;
        // push 8 vertices on cube
        mesh.vertices.emplace_back(Vertex{glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(1.0f,  1.0f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(-1.0f,  1.0f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(1.0f,  -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(1.0f,  -1.0f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(-1.0f,  -1.0f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(-1.0f,  -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.indices = {0, 3, 4, 3, 7, 4, 3, 2, 7, 2, 6, 7, 2, 1, 6, 1, 5, 6, 1, 0, 5, 0, 4, 5, 0, 1, 2, 0, 2, 3, 4, 7, 6, 4, 6, 5};

        return mesh;
    }

    static Mesh createUIMesh(const std::string &bg_img) {
        auto mesh = Mesh{};
        // mesh->mMaterial = std::make_shared<Material>();
        // mesh->mMaterial->updateTexture(TextureType::Albedo, bg_img);

        // push 8 vertices on cube
        mesh.vertices.emplace_back(Vertex{glm::vec3(0, 0, 0), glm::vec3(0.0f), glm::vec2(0, 0), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(0, 1, 0), glm::vec3(0.0f), glm::vec2(0, 1), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(1, 1, 0), glm::vec3(0.0f), glm::vec2(1, 1), glm::vec3(0.0f), glm::vec3(0.0f)});
        mesh.vertices.emplace_back(Vertex{glm::vec3(1, 0, 0), glm::vec3(0.0f), glm::vec2(1, 0), glm::vec3(0.0f), glm::vec3(0.0f)});

    	mesh.indices = {0, 2, 1, 0, 3, 2};

        // auto model = std::make_shared<Model>( std::vector<std::shared_ptr<Mesh>>{mesh});
        return mesh;
    }

    static std::shared_ptr<ShaderDesc> UIShader() {
        auto res = std::make_shared<ShaderDesc>();

        res->transform_inputs[TransformType::MVP] = "MVP";

        res->mat_inputs[MaterialType::AlbedoType] = "background";

        res->shader = {
            "UI",
            "./resources/shaders/ui.vert",
            "./resources/shaders/ui.frag"
        };
        return res;
    }

}



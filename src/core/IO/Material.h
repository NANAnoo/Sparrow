/*
 * Created by elecfrog@du
 * 2023/02/16
 * Stay Empty for API specification
 */

#pragma once
#include "Utils/MacroUtils.h"
#include "Maths/MathUtils.hpp"

#include "IO/RHI/Shader.h"
#include "IO/RHI/Texture.h"

namespace SPW
{
    // TODO: Define PBR Workflow
    const float PBR_WORKFLOW_SEPARATE_TEXTURES  = 0.0f;
    const float PBR_WORKFLOW_METALLIC_ROUGHNESS = 1.0f;
    const float PBR_WORKFLOW_SPECULAR_GLOSINESS = 2.0f;

    struct MaterialProperties
    {
        // Basic Material Properties without PBR
        glm::vec3 diffuse  = glm::vec3(1.0f,1.0f,1.0f);
        glm::vec3 specular = glm::vec3(1.0f,1.0f,1.0f);
        glm::vec3 ambient  = glm::vec3(0.1f,0.1f,0.1f);
        float shininess = 1000.0f;
        float IOR = 1.0f;
        int  illum = 5;

        // TODO: Define PBR Workflow
        glm::vec4 albedoColour   = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        float roughness          = 0.7f;
        float metallic           = 0.7f;
        float emissive           = 0.0f;
        float albedoMapFactor    = 1.0f;
        float metallicMapFactor  = 1.0f;
        float roughnessMapFactor = 1.0f;
        float normalMapFactor    = 1.0f;
        float emissiveMapFactor  = 1.0f;
        float occlusionMapFactor = 1.0f;
        float alphaCutoff        = 0.4f;
        float workflow           = PBR_WORKFLOW_SEPARATE_TEXTURES;
        float padding            = 0.0f;
    };

    struct PBRMataterialTextures
    {
        // TODO: Define PBR Workflow
        SharedPtr<Texture2D> albedo;
        SharedPtr<Texture2D> normal;
        SharedPtr<Texture2D> metallic;
        SharedPtr<Texture2D> roughness;
        SharedPtr<Texture2D> ao;
        SharedPtr<Texture2D> emissive;
    };

    class Material
    {
    public:
        Material(); // Initalize non-parameter constructor with parameter list
        Material(SharedPtr<Shader>& shader, const MaterialProperties& properties = MaterialProperties(), const PBRMataterialTextures& textures = PBRMataterialTextures());

        ~Material();

        // Setup MaterialProperties
        void SetMaterialProperites(MaterialProperties&& properties)
        {
            m_MaterialProperties = properties;
        }

        [[nodiscard]] MaterialProperties GetMaterialProperites() const
        {
            return m_MaterialProperties;
        }


        // Setup Shaders
        void SetShader(const SharedPtr<Shader>& _shader)
        {
            m_Shader = _shader;
            //TODO: Texture Upate
        }

        [[nodiscard]] SharedPtr<Shader> GetShader() const 
        {
            return m_Shader;
        }

        // Setup Textures
        void SetAlbedoTexture(const std::filesystem::path& _filePath)
        {
            m_PBRMaterialTextures.albedo = std::make_shared<Texture2D>(_filePath);
        }

        [[nodiscard]] std::shared_ptr<Texture2D> GetAlbedoTexture() const
        {
            return m_PBRMaterialTextures.albedo;
        }

        // TODO: UpateTextures

    private:
        PBRMataterialTextures m_PBRMaterialTextures;
        SharedPtr<Shader> m_Shader;
        MaterialProperties m_MaterialProperties;

    };
}
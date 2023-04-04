//
// Created by sgsdi on 2023/2/22.
//

#ifndef SPARROW_MATERIAL_H
#define SPARROW_MATERIAL_H

#include <string>
#include <map>
#include <vector>
#include <glm/glm.hpp>
namespace SPW
{
    struct MetallicProperties
    {
        float baseColorFactor{};
        float metallicFactor{};
        float roughnessFactor{};
        float anisotropyFactor{};
    };

    struct GlossinessProperties
    {
        float specularFactor{};
        float glossinessFactor{};
    };

	struct SheenProperties
    {
        float sheenFactor{};
        float sheenRoughnessFactor{};
    };

	struct ClearCoatProperties
    {
        float clearcoatFactor{};
        float clearcoatRoughnessFactor{};
    };

    struct VolumeProperties
    {
        float volumeThicknessFactor{};
        float volumeAttenuationDistance{};
    };

    struct MaterialProperties
    {
        glm::vec4 diffuseColor;
        glm::vec4 specularColor;
        glm::vec4 ambientColor;
        glm::vec4 emissiveColor;
        glm::vec4 transportColor;
        glm::vec4 refectiveColor;

        float shininess{};
        float opacity{};
        float transparentFactor{};
        float bumpScaling{};
        float refelectivity{};
        float shininessStrength{};
        float refracti{};

        MetallicProperties metallicProperties;
        GlossinessProperties glossinessProperties;
        SheenProperties sheenProperties;
        ClearCoatProperties clearCoatProperties;
        float emissiveIntensity{};
        VolumeProperties volumeProperties;
        float transmissionFactor{};

    };


    enum class TextureType
    {
        Albedo = 0,
        Normal,
        Metalness,
        Roughness,
        AmbientOcclusion,
        Unknown = 18,
    };


    class Material
    {
    public:
        std::string m_Name;
        std::map<TextureType, std::string> TextureMap;
        std::vector<std::string> cubeMapTextures;
        MaterialProperties m_Properties;

        void updateTexture(TextureType type,std::string texturePath)
        {
            if(texturePath=="")
            {
                TextureMap.erase(type);
                return;
            }

            TextureMap[type] = texturePath;
        }

    };
}

#endif //SPARROW_MATERIAL_H


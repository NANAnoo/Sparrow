/*
 * @date   2023-04-09
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include "glm/glm.hpp"
#include "Asset/Asset.hpp"

namespace SPW
{
	struct MaterialAttributes
	{
		// albedo as vec3
		glm::vec3 albedoConstant = glm::vec3(1.f);

		float metallicConstant{};
		float roughnessConstant{};
		float AOConstant{};

		// diffuse, specular, ambient, and emissive as vec3
		glm::vec3 diffuse = glm::vec3(1.f);
		glm::vec3 specular = glm::vec3(1.f);
		glm::vec3 ambient = glm::vec3(0.f);
		glm::vec3 emissive = glm::vec3(0.f);

		// transparent and refection as vec3
		glm::vec3 transparent;
		glm::vec3 refection;

		float emissiveIntensity{};
		float specularPower{};
		float transparentIntensity{};
		float refractionIntensity{};

		template <class Archive> void serialize(Archive &ar)
		{
			ar(
				cereal::make_nvp("albedoConstant", albedoConstant),
				cereal::make_nvp("metallicConstant", metallicConstant),
				cereal::make_nvp("AOConstant", AOConstant),
				cereal::make_nvp("diffuse", diffuse),
				cereal::make_nvp("specular", specular),
				cereal::make_nvp("ambient", ambient),
				cereal::make_nvp("emissive", emissive),
				cereal::make_nvp("transparent", transparent),
				cereal::make_nvp("refection", refection),
				cereal::make_nvp("emissiveIntensity", emissiveIntensity),
				cereal::make_nvp("specularPower", specularPower),
				cereal::make_nvp("transparentIntensity", transparentIntensity),
				cereal::make_nvp("refractionIntensity", refractionIntensity)
			);
		}
	};

	enum class TextureMapType
	{
		Albedo = 0,
		Normal,
		Metalness,
		Roughness,
		AmbientOcclusion,
		Unknown = 18,
	};

	struct MaterialData
	{
		std::string ID;
		std::map<TextureMapType, std::string> m_TextureIDMap;
		MaterialAttributes m_Properties;

		template <class Archive> void serialize(Archive &ar)
		{
			ar(
				cereal::make_nvp("id", ID),
				cereal::make_nvp("textureMap", m_TextureIDMap),
				cereal::make_nvp("property", m_Properties)
			);
		}
	};

}

// 添加对TextureType枚举的序列化支持
namespace cereal
{
	template <class Archive>
	std::string save_minimal(const Archive&, const SPW::TextureMapType& t)
	{
		switch (t)
		{
			case SPW::TextureMapType::Albedo: return "Albedo";
			case SPW::TextureMapType::Normal: return "Normal";
			case SPW::TextureMapType::Metalness: return "Metalness";
			case SPW::TextureMapType::Roughness: return "Roughness";
			case SPW::TextureMapType::AmbientOcclusion: return "AmbientOcclusion";
			case SPW::TextureMapType::Unknown: return "Unknown";
			default: throw std::invalid_argument("Invalid TextureType");
		}
	}

	template <class Archive>
	void load_minimal(const Archive&, SPW::TextureMapType& t, const std::string& value)
	{
		if (value == "Albedo") t = SPW::TextureMapType::Albedo;
		else if (value == "Normal") t = SPW::TextureMapType::Normal;
		else if (value == "Metalness") t = SPW::TextureMapType::Metalness;
		else if (value == "Roughness") t = SPW::TextureMapType::Roughness;
		else if (value == "AmbientOcclusion") t = SPW::TextureMapType::AmbientOcclusion;
		else if (value == "Unknown") t = SPW::TextureMapType::Unknown;
		else throw std::invalid_argument("Invalid TextureType");
	}
} // namespace cereal
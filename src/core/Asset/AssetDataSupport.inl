/*
 * @date   2023-04-09
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once



// ----------- GLM Atom Data Types -------------------
#include <glm/glm.hpp>

namespace glm
{
	template<class Archive, class T>
	void serialize(Archive& archive, glm::vec<2, T, glm::defaultp>& v)
	{
		archive(cereal::make_nvp("x", v.x),
				cereal::make_nvp("y", v.y));
	}

	template<class Archive, class T>
	void serialize(Archive& archive, glm::vec<3, T, glm::defaultp>& v)
	{
		archive(cereal::make_nvp("x", v.x),
				cereal::make_nvp("y", v.y),
				cereal::make_nvp("z", v.z));
	}

	template<class Archive, class T>
	void serialize(Archive& archive, glm::vec<4, T, glm::defaultp>& v)
	{
		archive(cereal::make_nvp("x", v.x),
				cereal::make_nvp("y", v.y),
				cereal::make_nvp("z", v.z),
				cereal::make_nvp("w", v.w));
	}

	// glm matrices serialization
	template<class Archive, class T>
	void serialize(Archive& archive, glm::mat<2, 2, T, glm::defaultp>& m) { archive(m[0], m[1]); }

	template<class Archive, class T>
	void serialize(Archive& archive, glm::mat<3, 3, T, glm::defaultp>& m) { archive(m[0], m[1], m[2]); }

	template<class Archive, class T>
	void serialize(Archive& archive, glm::mat<4, 4, T, glm::defaultp>& m) { archive(m[0], m[1], m[2], m[3]); }

	template<class Archive, class T>
	void serialize(Archive& archive, glm::qua<T, glm::defaultp>& q)
	{
		archive(cereal::make_nvp("x", q.x),
				cereal::make_nvp("y", q.y),
				cereal::make_nvp("z", q.z),
				cereal::make_nvp("w", q.w));
	}
}// namespace glm


// ----------- ENUM Type Support -------------------
namespace cereal
{
	template <class Archive>
	std::string save_minimal(const Archive&, const SPW::AssetType& t)
	{
		switch (t)
		{
			case SPW::AssetType::StaticModel: return "StaticModel";
			case SPW::AssetType::AnimatedModel: return "AnimatedModel";
			case SPW::AssetType::Mesh: return "Mesh";
			case SPW::AssetType::Material: return "Material";
			case SPW::AssetType::AnimationClip: return "AnimationClip";
			case SPW::AssetType::Skeleton: return "Skeleton";
			default: throw std::invalid_argument("Invalid AssetType");
		}
	}

	template <class Archive>
	void load_minimal(const Archive&, SPW::AssetType& t, const std::string& value)
	{
		if (value == "StaticModel") t = SPW::AssetType::StaticModel;
		else if (value == "AnimatedModel") t = SPW::AssetType::AnimatedModel;
		else if (value == "Mesh") t = SPW::AssetType::Mesh;
		else if (value == "Material") t = SPW::AssetType::Material;
		else if (value == "AnimationClip") t = SPW::AssetType::AnimationClip;
		else if (value == "Skeleton") t = SPW::AssetType::Skeleton;
		else throw std::invalid_argument("Invalid AssetType");
	}

} // namespace cereal
/*
 * @date   2023-04-17
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include "AnimationClip.h"
// #include "Mesh.h"

namespace SPW
{
	struct BoneInfo
	{
		int32_t boneID = -1;
		std::string name;
		glm::mat4 offsetMatrix;

		template <class Archive>
		inline void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("boneID", boneID),
				cereal::make_nvp("name", name),
				cereal::make_nvp("offsetMatrix", offsetMatrix)
			);
		}
	};

	struct HierarchyNode
	{
		glm::mat4 transformation;
		std::string name;
		std::vector<HierarchyNode> children;
		// std::vector<uint32_t> meshIndices;

		template <class Archive>
		inline void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("transformation", transformation),
				cereal::make_nvp("name", name),
				cereal::make_nvp("children", children)
			);
		}
	};

	struct VertBoneMap
	{
		// Buffer information
		std::vector<uint32_t> startIndex;
		std::vector<uint32_t> count;
		std::vector<float> weights;
		std::vector<uint32_t> boneID;
		int boneCount;

		template <class Archive>
		inline void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("startIndex", startIndex),
				cereal::make_nvp("count", count),
				cereal::make_nvp("weights", weights),
				cereal::make_nvp("boneID", boneID),
				cereal::make_nvp("boneCount", boneCount)
			);
		}
	};

	struct Skeleton
	{
    public:
		std::vector<AnimationClip> animClips;
		std::unordered_map<std::string, BoneInfo> boneMap;
		HierarchyNode hierarchy;
		VertBoneMap vertexWeightMap;

		template <class Archive>
		inline void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("animClips", animClips),
				cereal::make_nvp("boneMap", boneMap),
				cereal::make_nvp("hierarchy", hierarchy),
				cereal::make_nvp("vertexWeightMap", vertexWeightMap)
			);
		}
	};

	inline glm::mat4 CalculateInterpolatedMatrix(AnimationNode* node, float currentTime)
	{
		//Get transform here
		glm::mat4 position = node->InterpolatePosition(currentTime);
		glm::mat4 rotation = node->InterpolateRotation(currentTime);
		glm::mat4 scaling = node->InterpolateScaling(currentTime);

		return position * rotation * scaling;
	}

	inline AnimationNode* FindAnimationNode(AnimationClip& anim_clip, const std::string& name)
	{
		for (auto& node : anim_clip.animNodes)
		{
			if (node.name == name)
			{
				return &node;
			}
		}
		return nullptr;
	}
}

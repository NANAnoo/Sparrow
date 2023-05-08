/*
 * @date   2023-04-19
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Asset/Asset.hpp"

namespace SPW
{
	enum class AnimationState
	{
		Start, Stopped, Paused, OnPlaying
	};

	enum class AnimationAction
	{
		Reset, Swap, Pause, Resume
	};

	struct KeyPosition
	{
		double time;
		glm::vec3 position;

		template <class Archive>
		inline void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("time", position));
		}
	};

	struct KeyRotation
	{
		double time;
		glm::quat rotation;

		template <class Archive>
		inline void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("time", rotation));
		}

	};

	struct KeyScale
	{
		double time;
		glm::vec3 scaling;

		template <class Archive>
		inline void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("time", scaling));
		}
	};

	struct KeyFrame
	{
		double time;
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 sacling;

		template <class Archive>
		inline void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("time", time),
				cereal::make_nvp("position", position),
				cereal::make_nvp("rotation", rotation),
				cereal::make_nvp("sacling", sacling)
			);
		}

	};

	struct MatrixPerFrame
	{
		std::vector<glm::mat4> transformMatrix;
		float timeStamp;

		MatrixPerFrame(int boneSize, float t)
		{
			transformMatrix.resize(boneSize, glm::mat4(1.0f));

			timeStamp = t;
		}

		MatrixPerFrame() = default;

		template <class Archive>
		inline void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("timeStamp", timeStamp),
				cereal::make_nvp("transformMatrix", transformMatrix)
			);
		}
	};

	struct AnimationNode
	{
		std::string name;
		std::vector<KeyFrame> keyFrames;
		std::vector<KeyPosition> positionKeys;
		std::vector<KeyRotation> rotationKeys;
		std::vector<KeyScale> scalingKeys;

		size_t GetPositionIndex(float animationTime) const;
		size_t GetRotationIndex(float animationTime) const;
		size_t GetScaleIndex(float animationTime) const;

		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;

		glm::mat4 InterpolatePosition(float animationTime) const;
		glm::mat4 InterpolateRotation(float animationTime) const;
		glm::mat4 InterpolateScaling(float animationTime) const;

		template <class Archive>
		inline void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("name", name),
				cereal::make_nvp("keyFrames", keyFrames),
				cereal::make_nvp("positionKeys", positionKeys),
				cereal::make_nvp("rotationKeys", rotationKeys),
				cereal::make_nvp("scalingKeys", scalingKeys)
			);
		}
	};

	struct AnimationClip
	{
		std::string name;
		double duration;
		uint32_t FPS;
		uint32_t frameCount;
		std::vector<AnimationNode> animNodes;
		std::vector<MatrixPerFrame> matrixPerFrames;
		std::vector<glm::mat4> boneMatrices;

		bool isLoop = false;
		bool bUpdate = false;
		double currentTime = 0.0;
		AnimationState state = AnimationState::Stopped;

		template <class Archive>
		inline void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("name", name),
				cereal::make_nvp("duration", duration),
				cereal::make_nvp("FPS", FPS),
				cereal::make_nvp("frameCount", frameCount),
				cereal::make_nvp("animNodes", animNodes),
				cereal::make_nvp("matrixPerFrames", matrixPerFrames),
				cereal::make_nvp("boneMatrices", boneMatrices)
			);
		}

		void play();
		void startFromStop();
		void resume();
		void pause();
		void stop();
		void setState(AnimationState state);
		void resetTime();
		void update(float dt);
	};

}

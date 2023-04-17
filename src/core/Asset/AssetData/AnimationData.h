/*
 * @date   2023-04-17
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "glm/gtx/quaternion.hpp"

namespace SPW
{
	struct Skeleton;
	struct KeyFrame;
	struct AnimationClip;
	struct AnimationNode;
	struct BoneInfo;
	struct Weight;
	struct HierarchyNode;
	struct MatrixPerFrame;
	struct VerMapBone;

	//Begin a new Animation
	//1:
	enum class AnimationState { Start, Stopped, Paused, OnPlaying };
	enum class AnimationAction { Reset, Swap, Pause, Resume };

	struct Weight
	{
		uint32_t vertexID;
		float value;
	};

	struct MatrixPerFrame
	{
		MatrixPerFrame(int boneSize, float timeStamp)
		{
			transformMatrix.resize(boneSize, glm::mat4(1.0f));
			this->timeStamp = timeStamp;
		}

		MatrixPerFrame() = default;

		std::vector<glm::mat4> transformMatrix;
		float timeStamp;
	};

	struct BoneInfo
	{
		int32_t boneID   = -1;
		int32_t parentID = -1;

		std::vector<uint32_t> childrenIDs = std::vector<uint32_t>{};

		std::string name{};
		uint32_t		mNumWeights;
		std::vector<Weight> weights;
		glm::mat4 offsetMatrix;

		BoneInfo() = default;

		BoneInfo(uint32_t _boneID, std::string&& _name, uint32_t _mNumWeights, std::vector<Weight>&& _weights,
			glm::mat4 _offsetMatrix)
			: boneID(_boneID), name(_name), mNumWeights(_mNumWeights), weights(_weights), offsetMatrix(_offsetMatrix)
		{
		}
	};

	struct HierarchyNode
	{
		glm::mat4   transformation;
		std::string name;
		std::vector<HierarchyNode> children;
	};


	struct AnimationClip
	{
		std::string name = "Scene";
		double	 duration;
		uint32_t FPS;
		uint32_t frameCount;
		HierarchyNode root;
		std::vector<AnimationNode>	nodeAnimations;
		std::vector<MatrixPerFrame>	boneBindings;

		bool isLoop			 = false;
		bool bUpdate		 = false;
		float currentTime	 = 0.f;
		AnimationState state = AnimationState::Stopped;

		std::vector<glm::mat4> finalBoneMatrices;


		void play()
		{
			if (frameCount > 0)
			{
				if (state == SPW::AnimationState::Stopped)
				{
					setState(AnimationState::Start);
					resetTime();
					startFromStop();
				}
			}
		}

		void startFromStop()
		{
			if (frameCount > 0)
			{
				if (state == SPW::AnimationState::Start)
				{
					setState(AnimationState::OnPlaying);
					bUpdate = true;
				}
			}
		}


		void resume()
		{
			if (frameCount > 0)
			{
				if (state == SPW::AnimationState::Paused)
				{
					setState(SPW::AnimationState::OnPlaying);
					bUpdate = true;
				}
			}
		}

		void pause()
		{
			if (frameCount > 0)
			{
				if (state == SPW::AnimationState::OnPlaying)
				{
					setState(SPW::AnimationState::Paused);
					bUpdate = false;
				}
			}
		}

		void stop()
		{
			if (frameCount > 0)
			{
				if (state != SPW::AnimationState::Stopped)
				{
					setState(SPW::AnimationState::Stopped);
					bUpdate = false;
				}
			}
		}

		void setState(AnimationState state)
		{
			this->state = state;
		}

		void resetTime()
		{
			currentTime = 0.0f;
		}

		void update(float dt)
		{
			if (bUpdate)
			{
				currentTime += FPS * dt * 0.5f;
				currentTime = fmod(currentTime, duration);

				int preIndex;
				int index;
				for (int i = 0; i < boneBindings.size() - 1; ++i)
				{
					if (boneBindings[i + 1].timeStamp > currentTime)
					{
						index = i + 1;
						preIndex = i;
						break;
					}
				}

				float preTime = boneBindings[preIndex].timeStamp;
				float nextTime = boneBindings[index].timeStamp;
				float midTime = currentTime;

				float left = midTime - preTime;
				float frameDua = nextTime - preTime;
				float factor = left / frameDua;

				for (int i = 0; i < finalBoneMatrices.size(); ++i)
				{
					glm::mat4 preTransform = boneBindings[preIndex].transformMatrix[i];
					glm::mat4 lastTransform = boneBindings[index].transformMatrix[i];

					glm::vec3 translation, scaling;
					glm::quat rotation;
					glm::vec3 skew;
					glm::vec4 perspective;
					glm::decompose(preTransform, scaling, rotation, translation, skew, perspective);

					glm::vec3 translation1, scaling1;
					glm::quat rotation1;
					glm::vec3 skew1;
					glm::vec4 perspective1;
					glm::decompose(lastTransform, scaling1, rotation1, translation1, skew1, perspective1);

					glm::vec3 finalPosition = glm::mix(translation, translation1
						, factor);

					glm::quat finalRotation = glm::slerp(rotation, rotation1
						, factor);

					glm::vec3 finalScale = glm::mix(scaling, scaling1
						, factor);

					finalBoneMatrices[i] = glm::translate(glm::mat4(1.0f), finalPosition) * glm::toMat4(finalRotation) *
						glm::scale(glm::mat4(1.0f), finalScale);
				}
			}
		}
	};

	struct KeyPosition
	{
		double time;
		glm::vec3 position;
	};

	struct KeyRotation
	{
		double time;
		glm::quat rotation;
	};

	struct KeyScale
	{
		double time;
		glm::vec3 scaling;
	};

	struct KeyFrame
	{
		double time;
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 sacling;
	};

	struct AnimationNode
	{
		std::string nodeName;
		uint32_t boneID;
		std::vector<KeyFrame> keyFrames;

		std::vector<KeyPosition> m_Position;
		std::vector<KeyRotation> m_Rotation;
		std::vector<KeyScale> m_Scaling;


		int GetPositionIndex(float animationTime)
		{
			for (int index = 0; index < m_Position.size() - 1; ++index)
			{
				if (animationTime < m_Position[index + 1].time)
					return index;
			}
			return 0;
			assert(0);
		}

		int GetRotationIndex(float animationTime)
		{
			for (int index = 0; index < m_Rotation.size() - 1; ++index)
			{
				if (animationTime < m_Rotation[index + 1].time)
					return index;
			}
			return 0;
			assert(0);
		}

		int GetScaleIndex(float animationTime)
		{
			for (int index = 0; index < m_Scaling.size() - 1; ++index)
			{
				if (animationTime < m_Scaling[index + 1].time)
					return index;
			}
			return 0;
			assert(0);
		}

		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
		{
			float scaleFactor = 0.0f;
			float midWayLength = animationTime - lastTimeStamp;
			float framesDiff = nextTimeStamp - lastTimeStamp;
			scaleFactor = midWayLength / framesDiff;
			return scaleFactor;
		}


		glm::mat4 InterpolatePosition(float animationTime)
		{
			if (1 == m_Position.size())
				return glm::translate(glm::mat4(1.0f), m_Position[0].position);

			int p0Index = GetPositionIndex(animationTime);
			int p1Index = p0Index + 1;
			float scaleFactor = GetScaleFactor(m_Position[p0Index].time,
				m_Position[p1Index].time, animationTime);

			glm::vec3 finalPosition = glm::mix(m_Position[p0Index].position, m_Position[p1Index].position
				, scaleFactor);
			return glm::translate(glm::mat4(1.0f), finalPosition);
		}

		glm::mat4 InterpolateRotation(float animationTime)
		{
			if (1 == m_Rotation.size())
			{
				auto rotation = glm::normalize(m_Rotation[0].rotation);
				return glm::toMat4(rotation);
			}

			int p0Index = GetRotationIndex(animationTime);
			int p1Index = p0Index + 1;
			float scaleFactor = GetScaleFactor(m_Rotation[p0Index].time,
				m_Rotation[p1Index].time, animationTime);
			glm::quat finalRotation = glm::slerp(m_Rotation[p0Index].rotation, m_Rotation[p1Index].rotation
				, scaleFactor);
			finalRotation = glm::normalize(finalRotation);
			return glm::toMat4(finalRotation);
		}

		glm::mat4 InterpolateScaling(float animationTime)
		{
			if (1 == m_Scaling.size())
				return glm::scale(glm::mat4(1.0f), m_Scaling[0].scaling);

			int p0Index = GetScaleIndex(animationTime);
			int p1Index = p0Index + 1;
			float scaleFactor = GetScaleFactor(m_Scaling[p0Index].time,
				m_Scaling[p1Index].time, animationTime);
			glm::vec3 finalScale = glm::mix(m_Scaling[p0Index].scaling, m_Scaling[p1Index].scaling
				, scaleFactor);
			return glm::scale(glm::mat4(1.0f), finalScale);
		}
	};

	// TODO: Insert it into asset data
	struct Skeleton
	{
		std::vector<AnimationClip>				  animClips;
		std::unordered_map<std::string, BoneInfo> boneMap;
		// std::vector<MatrixPerFrame>				  boneMatrices;

		// TODO: try to remove it
		std::vector<BoneInfo> m_Bones;
		HierarchyNode		  root;
	};



	struct VerMapBone
	{
		//Vertex -> <BoneID> -> (stores Index of finalBoneMatrix)
		std::vector<uint32_t> boneID;
		//Weight -> <double) -> (stores corresponding weight; (boneID[0] - Weight[0]) )
		std::vector<float> weight;
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
		for (auto& node : anim_clip.nodeAnimations)
		{
			if (node.nodeName == name)
			{
				return &node;
			}
		}
		return nullptr;
	}

}

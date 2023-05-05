//
// Created by dudu on 2023/4/19.
//
#include "AnimationClip.h"

namespace SPW
{
	size_t AnimationNode::GetPositionIndex(float animationTime) const
	{
		for (size_t index = 0; index < positionKeys.size() - 1; ++index)
		{
			if (animationTime < positionKeys[index + 1].time)
				return index;
		}
		return 0;
	}

	size_t AnimationNode::GetRotationIndex(float animationTime) const
	{
		for (size_t index = 0; index < rotationKeys.size() - 1; ++index)
		{
			if (animationTime < rotationKeys[index + 1].time)
				return index;
		}
		return 0;
	}

	size_t AnimationNode::GetScaleIndex(float animationTime) const
	{
		for (size_t index = 0; index < scalingKeys.size() - 1; ++index)
		{
			if (animationTime < scalingKeys[index + 1].time)
				return index;
		}
		return 0;
	}

	float AnimationNode::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const
	{
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;
		float framesDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;
		return scaleFactor;
	}

	glm::mat4 AnimationNode::InterpolatePosition(float animationTime) const
	{
		if (1 == positionKeys.size())
			return glm::translate(glm::mat4(1.0f), positionKeys[0].position);

		int p0Index = GetPositionIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(positionKeys[p0Index].time,
			positionKeys[p1Index].time, animationTime);

		glm::vec3
			finalPosition = glm::mix(positionKeys[p0Index].position, positionKeys[p1Index].position, scaleFactor);
		return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	glm::mat4 AnimationNode::InterpolateRotation(float animationTime) const
	{
		if (1 == rotationKeys.size())
		{
			const auto rotation = glm::normalize(rotationKeys[0].rotation);
			return glm::toMat4(rotation);
		}

		const int p0Index = GetRotationIndex(animationTime);
		const int p1Index = p0Index + 1;
		const float scaleFactor = GetScaleFactor(rotationKeys[p0Index].time,
			rotationKeys[p1Index].time, animationTime);
		glm::quat
			finalRotation = glm::slerp(rotationKeys[p0Index].rotation, rotationKeys[p1Index].rotation, scaleFactor);
		finalRotation = glm::normalize(finalRotation);
		return glm::toMat4(finalRotation);
	}

	glm::mat4 AnimationNode::InterpolateScaling(float animationTime) const
	{
		if (1 == scalingKeys.size())
			return glm::scale(glm::mat4(1.0f), scalingKeys[0].scaling);

		const int p0Index = GetScaleIndex(animationTime);
		const int p1Index = p0Index + 1;
		const float scaleFactor = GetScaleFactor(scalingKeys[p0Index].time,
			scalingKeys[p1Index].time, animationTime);
		const glm::vec3
			finalScale = glm::mix(scalingKeys[p0Index].scaling, scalingKeys[p1Index].scaling, scaleFactor);
		return glm::scale(glm::mat4(1.0f), finalScale);
	}



		void AnimationClip::play()
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

		void AnimationClip::startFromStop()
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

		void AnimationClip::resume()
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

		void AnimationClip::pause()
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

		void AnimationClip::stop()
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

		void AnimationClip::setState(AnimationState state)
		{
			this->state = state;
		}

		void AnimationClip::resetTime()
		{
			currentTime = 0.0f;
		}

		void AnimationClip::update(float dt)
		{
			if (bUpdate)
			{
				currentTime += FPS * dt * 0.5f;
				currentTime = fmod(currentTime, duration);

				int preIndex;
				int index;
				for (int i = 0; i < matrixPerFrames.size() - 1; ++i)
				{
					if (matrixPerFrames[i + 1].timeStamp > currentTime)
					{
						index = i + 1;
						preIndex = i;
						break;
					}
				}

				float preTime = matrixPerFrames[preIndex].timeStamp;
				float nextTime = matrixPerFrames[index].timeStamp;
				float midTime = currentTime;

				float left = midTime - preTime;
				float frameDua = nextTime - preTime;
				float factor = left / frameDua;

				for (int i = 0; i < boneMatrices.size(); ++i)
				{
					glm::mat4 preTransform = matrixPerFrames[preIndex].transformMatrix[i];
					glm::mat4 lastTransform = matrixPerFrames[index].transformMatrix[i];

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

					glm::vec3 finalPosition = glm::mix(translation, translation1, factor);

					glm::quat finalRotation = glm::slerp(rotation, rotation1, factor);

					glm::vec3 finalScale = glm::mix(scaling, scaling1, factor);

					boneMatrices[i] = glm::translate(glm::mat4(1.0f), finalPosition) * glm::toMat4(finalRotation) *
						glm::scale(glm::mat4(1.0f), finalScale);
				}
			}
		}
}


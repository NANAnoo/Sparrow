#pragma once
#include "glm/gtx/quaternion.hpp"

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace SPW {
    struct KeyFrame;
    struct AnimationClip;
    struct AnimationNode;
    struct BoneInfo;
    struct Weight;

    struct Weight {
        uint32_t vertexID;
        float value;
    };

    struct BoneInfo {
        uint32_t boneID;
        int32_t parentID{-1};
        std::vector<uint32_t> childrenIDs = std::vector<uint32_t>{};

        std::string name;
        uint32_t mNumWeights;
        std::vector<Weight> weights;
        glm::mat4 offsetMatrix;

        BoneInfo(uint32_t _boneID, std::string &&_name, uint32_t _mNumWeights, std::vector<Weight> &&_weights, glm::mat4 _offsetMatrix)
                : boneID(_boneID), name(_name), mNumWeights(_mNumWeights), weights(_weights), offsetMatrix(_offsetMatrix) {}
    };

    struct AssimpNodeData
    {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;

    };


    struct AnimationClip {
        std::string name;
        double duration;
        uint32_t FPS;
        uint32_t frameCount;
        AssimpNodeData aiRootNode;
        std::vector<AnimationNode> nodeAnimations;
    };



    struct KeyPosition{
        double time;
        glm::vec3 position;
    };
    struct KeyRotation{
        double time;
        glm::quat rotation;
    };

    struct KeyScale{
        double time;
        glm::vec3  scaling;
    };

    struct KeyFrame {
        double time;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 sacling;
    };

    struct AnimationNode {
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
            float scaleFactor = GetScaleFactor(m_Rotation[p0Index].time ,
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
}
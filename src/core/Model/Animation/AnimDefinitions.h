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

    struct AnimationNode {
        std::string nodeName;
        uint32_t boneID;
        std::vector<KeyFrame> keyFrames;
    };

    struct KeyFrame {
        double time;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 sacling;
    };
}
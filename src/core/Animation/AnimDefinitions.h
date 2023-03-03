#pragma once

struct KeyFrame;
struct AnimationClip;
struct AnimationNode;
struct BoneInfo;
struct Weight;

struct Weight
{
    uint32_t vertexID;
    float value;
};

struct BoneInfo
{
    uint32_t boneID;
    int32_t parentID{ -1 };
    std::vector<uint32_t> childrenIDs = std::vector<uint32_t>{};

    std::string name;
    uint32_t mNumWeights;
    std::vector<Weight> weights;
    glm::mat4 offsetMatrix;

    BoneInfo(std::string&& _name, uint32_t _mNumWeights, std::vector<Weight>&& _weights, glm::mat4 _offsetMatrix)
        :name(_name), mNumWeights(_mNumWeights), weights(_weights), offsetMatrix(_offsetMatrix)
    {	}
};

struct AnimationClip
{
    std::string name;
    double duration;
    uint32_t FPS;
    uint32_t frameCount;
    std::vector<AnimationNode> nodeAnimations;
};

struct AnimationNode
{
    std::string nodeName;
    std::vector<KeyFrame> keyFrames;
};

struct KeyFrame
{
    double time;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 sacling;
};
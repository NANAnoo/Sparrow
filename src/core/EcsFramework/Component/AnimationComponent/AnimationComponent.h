//
// Created by Shawwy on 2/24/2023.
//



#ifndef SPARROW_ANIMATIONCOMPONENT_H
#define SPARROW_ANIMATIONCOMPONENT_H

#include "../ComponentI.h"
#include "./Model/Animation/Skeleton.h"
#include <vector>
#include "glm/glm.hpp"
#include "Render/StorageBuffer.h"

//#include "Animation.h"
namespace SPW {

    enum class State {started,stopped};
    using ClipTransform = std::vector<std::vector<glm::mat4>>;
    using FlattenTransform = std::vector<glm::mat4>;

    struct VerMapBone
    {
        //Vertex -> <BoneID> -> (stores Index of finalBoneMatrix)
        std::vector<uint32_t> boneID;
        //Weight -> <double) -> (stores corresponding weight; (boneID[0] - Weight[0]) )
        std::vector<float> weight;
    };

    struct AnimationClipSSBO {
        std::shared_ptr<StorageBuffer> starts;
        std::shared_ptr<StorageBuffer> sizes;
        std::shared_ptr<StorageBuffer> boneIndices;
        std::shared_ptr<StorageBuffer> weights;
        std::shared_ptr<StorageBuffer> mats;
    };

    struct AnimationClipTransform
    {
        //TODO: 二维降成一维数组，
        // 如何找到第a个骨骼的第i和i+1帧？（ a * frameCount + i)
        ClipTransform finalKeyframeMatrices;
        FlattenTransform flattenTransform;
        int frameCount;
        std::string animName;
    };

    //For all vertices
    struct AnimBufferInfo
    {
        //Start index 只负责记录weights数组
        std::vector<uint32_t> startIndex;
        std::vector<uint32_t> size;
        std::vector<float> weights;
        std::vector<uint32_t> boneID;

        ClipTransform finalKeyframeMatrices;
        FlattenTransform flattenTransform;
        int frameCount;
        std::string animName;
    };



    class AnimationComponent : ComponentI
    {
    public:
        //Constructor
        AnimationComponent() = default;

        //Status related
        State state = State::stopped;
        bool bInitialized = false;
        bool bLoaded = false;
        std::string incomingAnimName;

        //Current
        float playSpeed = 1.0f;
        float currentTime;

        std::weak_ptr<AnimationClip> currentAnimation;

        std::shared_ptr<Skeleton> skeleton;

        std::vector<glm::mat4> finalBoneMatrices;

        std::vector<float*> finalBoneArray;

        // Used to
        std::vector<VerMapBone> verMapBone;

        AnimationClipSSBO current_clip;

        std::vector<AnimationClipTransform> finalKeyMatricesAllClips;


        AnimBufferInfo vertexBoneMap;
        int indices [2];
        float frameWeights[2];
    };


}

#endif //SPARROW_ANIMATIONCOMPONENT_H



//
// Created by Shawwy on 2/24/2023.
//



#ifndef SPARROW_ANIMATIONCOMPONENT_H
#define SPARROW_ANIMATIONCOMPONENT_H

#include "../ComponentI.h"
#include "Model/Animation/Skeleton.h"
#include "Model/Animation/AnimDefinitions.h"
#include <vector>
#include "glm/glm.hpp"
#include "Render/StorageBuffer.h"

//#include "Animation.h"
namespace SPW {

    //Start: Start an animation from 'Stopped' state or 'Paused' state
        //Reset the current time and change to OnPlaying
    //Stopped: Animation is stopped at time: 0
        //OnPlaying -> stop: bUpdate = false; (Swaping
    //Pause: Animation is stopped during playing
    //Onplaying: Animation is playing and keeps updating




    enum class AnimationState {Start,Stopped,Pause,OnPlaying};
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

    //For all vertices
    struct AnimBufferInfo
    {
        //Start index 只负责记录weights数组
        std::vector<uint32_t> startIndex;
        std::vector<uint32_t> size;
        std::vector<float> weights;
        std::vector<uint32_t> boneID;
    };


    class SPWAnimationDelegateI{
    public:
        virtual std::weak_ptr<AnimationClip> playAnim(const std::string &name, bool isLoop);
    };


    //SPW::Animation class
    //包装AnimationClip
    class SPWAnimation
    {
    public:
        SPWAnimation() = default;

        explicit SPWAnimation(std::shared_ptr<AnimationClip> clip,std::shared_ptr<Skeleton> skeleton)
        {
            animationClip = clip;
            animationName = clip->name;
            currentTime = 0.0f;
            duration = clip->duration;
            tickPerSecond = clip->FPS;
            finalBoneMatrices.resize(skeleton->m_Bones.size(),glm::mat4(1.0f));
        }

        void play(SPWAnimationDelegateI *delegateI)
        {

        }

        void resume()
        {
            if (!animationClip.expired())
            {
                //setState(paused)
            }
        }

        //void stop()


        void setState(AnimationState state)
        {



        }

        //void setPos()

        //bool needUpdate();

        bool isLoop;

        AnimationState state = AnimationState::Stopped;

    private:

        bool bUpdate;

        std::weak_ptr<AnimationClip> animationClip;

        std::string animationName;

        float currentTime;

        float duration;

        int tickPerSecond;

        std::vector<glm::mat4> finalBoneMatrices;
    };





    //SPW::Skeleton class
    //包装一个骨骼类


    class AnimationComponent : ComponentI
    {
    public:
        //Constructor
        AnimationComponent() = default;

        //Status related
        AnimationState state = AnimationState::Stopped;
        bool bInitialized = false;
        bool bLoaded = false;
        bool bHasAnim = false;
        std::string incomingAnimName;

        //Current state
        float playSpeed = 1.0f;
        float currentTime;
        std::weak_ptr<AnimationClip> currentAnimation;

        //Essential information

        FlattenTransform flattenTransform;
        std::shared_ptr<Skeleton> skeleton;


        //Buffer information
        AnimBufferInfo bufferInfo;
        AnimationClipSSBO current_clip;


        int indices [2];
        float frameWeights[2];
    };
}

#endif //SPARROW_ANIMATIONCOMPONENT_H



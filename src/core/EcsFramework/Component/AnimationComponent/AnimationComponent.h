//
// Created by Shawwy on 2/24/2023.
//



#ifndef SPARROW_ANIMATIONCOMPONENT_H
#define SPARROW_ANIMATIONCOMPONENT_H

#include "../ComponentI.h"
#include "/Animation/Skeleton.h"
#include <vector>
#include "glm/glm.hpp"
//#include "Animation.h"
namespace SPW {

    struct VerMapBone
    {
        //Vertex -> <BoneID> -> (stores Index of finalBoneMatrix)
        std::vector<uint32_t> boneID;
        //Weight -> <double) -> (stores corresponding weight; (boneID[0] - Weight[0]) )
        std::vector<float> weight;
    };

    enum class State {started,stopped};


    class AnimationComponent : ComponentI
    {
    public:
        //Constructor
        AnimationComponent();

        //Variables needed
        State state = State::stopped;
        bool bInitialized = false;
        float playSpeed = 1.0f;
        float currentTime;

        std::string incomingAnimName;

        std::weak_ptr<AnimationClip> currentAnimation;
        std::shared_ptr<Skeleton> skeleton;
        //Skeleton skeleton;

        std::vector<glm::mat4> finalBoneMatrices;
        std::vector<float*> finalBoneArray;
        std::vector<VerMapBone> verMapBone;
    };


}

#endif //SPARROW_ANIMATIONCOMPONENT_H



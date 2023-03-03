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

    class AnimationComponent : ComponentI
    {
    public:
        //Constructor
        AnimationComponent();

        //Variables needed
        bool bStarted = false;
        bool bStop = true;
        float playSpeed = 1.0f;

        float currentTime;
        float deltaTime;

        std::weak_ptr<AnimationClip> currentAnimation;
        Skeleton skeleton;
        std::vector<glm::mat4> finalBoneMatrices;
    };
}




#endif //SPARROW_ANIMATIONCOMPONENT_H



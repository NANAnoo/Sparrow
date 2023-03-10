//
// Created by Shawwy on 2/24/2023.
//

#ifndef SPARROW_ANIMATIONSYSTEM_H
#define SPARROW_ANIMATIONSYSTEM_H

#include "EcsFramework/System/SystemI.h"
#include "glm/glm.hpp"
#include "EcsFramework/Component/AnimationComponent/AnimationComponent.h"
#include "EcsFramework/Component/ModelComponent.h"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Scene.hpp"
#include <glm/glm/ext.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/euler_angles.hpp>


#include "Model/Animation/Skeleton.h"
#include "Model/Animation/AnimDefinitions.h"

#include <tuple>
#include <vector>


namespace SPW{
    class AnimationSystem : SystemI{

    public:
        explicit AnimationSystem(std::shared_ptr<Scene> &scene) : SystemI(scene){}
        using AnimatedEntity = std::tuple<AnimationComponent*, IDComponent*,ModelComponent*>;

        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;

    private:

        void precalculateTransform(AnimationComponent& animationComponent);


        void updateAnimation(std::string name,float dt,AnimationComponent& animationComponent);
        void updateFramesWeight(std::string name,float dt,AnimationComponent& animationComponent);

        void playAnimation(std::string name,float dt,AnimationComponent& animationComponent);
        void stopAnimation(AnimationComponent& animationComponent);
        void calculateBoneTransform(std::shared_ptr<BoneInfo>bone,
                                    glm::mat4 parrentTransform,
                                    AnimationComponent& animationComponent,
                                    float currentTime);

        void calcuKeyframeTransform(std::shared_ptr<BoneInfo>bone,
                                    glm::mat4 parrentTransform,
                                    AnimationComponent& animationComponent,
                                    uint32_t index,
                                    ClipTransform clip);
        void initializeComponent(AnimationComponent& animationComponent,ModelComponent& modelComponent);
        void changeMap(AnimationComponent& animationComponent, ModelComponent& modelComponent);
        void vertexBoneMapping(AnimationComponent& animationComponent,ModelComponent& modelComponent);


        std::shared_ptr<BoneInfo> findRootNode(AnimationComponent& animationComponent);
        AnimationNode findAnimationNode(std::string name,std::weak_ptr<AnimationClip> currentAnimation);
        std::shared_ptr<AnimationClip> findAnimation(std::string name,AnimationComponent& animationComponent);

        glm::mat4 getUpdatedTransform(AnimationNode node,float currentTime);
        glm::mat4 getKeyframeTransform(AnimationNode node,int index);

        KeyFrame interpolateKeyFrame(float playTime);
        int  getIndexOfCurrentFrame(float playTime);

        inline int getTickPerSecond(){return tickPerSecond;};
        int tickPerSecond;

    };
}



#endif //SPARROW_ANIMATIONSYSTEM_H

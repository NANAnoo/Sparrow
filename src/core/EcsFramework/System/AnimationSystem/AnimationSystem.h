//
// Created by Shawwy on 2/24/2023.
//

#ifndef SPARROW_ANIMATIONSYSTEM_H
#define SPARROW_ANIMATIONSYSTEM_H

#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/System/SystemI.h"
#include "glm/glm.hpp"
#include "EcsFramework/Component/AnimationComponent/AnimationComponent.h"
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
    class AnimationSystem : public SystemI{

    public:
        explicit AnimationSystem(std::shared_ptr<Scene> &scene) : SystemI(scene){}
        using AnimatedEntity = std::tuple<AnimationComponent*, IDComponent*,MeshComponent*>;

        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;

    private:


        void updateAnimation(std::string name,float dt,AnimationComponent& animationComponent);
        void playAnimation(std::string name,float dt,AnimationComponent& animationComponent);
        void stopAnimation(AnimationComponent& animationComponent);

        //TODO: bone is actually aiNode
        void calculateBoneTransform(const HierarchyNode* node,
                                    glm::mat4 parrentTransform,
                                    AnimationComponent& animationComponent,
                                    float currentTime);

        void initializeComponent(AnimationComponent& animationComponent,MeshComponent& MeshComponent);
        void changeMap(AnimationComponent& animationComponent, MeshComponent& MeshComponent);
        void vertexBoneMapping(AnimationComponent &animationComponent, MeshComponent &MeshComponent,
                               std::vector<VerMapBone> map);

        AnimationNode findAnimationNode(std::string name,std::weak_ptr<AnimationClip> currentAnimation);
        std::shared_ptr<AnimationClip> findAnimation(std::string name,AnimationComponent& animationComponent);

        glm::mat4 getUpdatedTransform(AnimationNode* node,float currentTime);

        //================================================================================================================
        glm::mat4 getKeyframeTransform(AnimationNode node,int index);

        KeyFrame interpolateKeyFrame(float playTime);
        int  getIndexOfCurrentFrame(float playTime);

        inline int getTickPerSecond(){return tickPerSecond;};

        void calcuKeyframeTransform(std::shared_ptr<BoneInfo> bone, glm::mat4 parrentTransform,
                                    AnimationComponent &animationComponent, uint32_t frameIndex,
                                    uint32_t boneIndex,
                                    std::weak_ptr<AnimationClip> Animclip);
        void updateFramesWeight(std::string name,float dt,AnimationComponent& animationComponent);
        void precalculateTransform(AnimationComponent& animationComponent);

        int tickPerSecond;

    };
}



#endif //SPARROW_ANIMATIONSYSTEM_H

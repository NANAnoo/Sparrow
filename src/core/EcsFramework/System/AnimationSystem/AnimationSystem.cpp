
// Created by Shawwy on 2/24/2023.
//

#include "AnimationSystem.h"
#include "EcsFramework/Component/MeshComponent.hpp"


void SPW::AnimationSystem::initial()
{

}
void SPW::AnimationSystem::beforeUpdate()
{
    ComponentGroup<SPW::AnimationComponent,SPW::IDComponent,SPW::MeshComponent> animatedGroup;
    locatedScene.lock()->forEachEntityInGroup
            (animatedGroup,
             [this,&animatedGroup](const Entity &entity)
             {
                 AnimatedEntity animatedEntity = entity.combinedInGroup(animatedGroup);
                 //Get animationComp and modelComp of this entity
                 auto animationComp = entity.component<SPW::AnimationComponent>();
                 auto modelComp = entity.component<SPW::MeshComponent>();

                 if (!animationComp->mapInitialize)
                 {
                     animationComp->initializeMapping(modelComp->assetName);
                 }

                 //Binding buffer
                 if (animationComp->SPW_AnimSSBO)
                 {
                     if (!animationComp->SPW_AnimSSBO->bBinding)
                     {
                         animationComp->SPW_AnimSSBO->bindingBuffer(modelComp);
                         animationComp->SPW_AnimSSBO->updateStaticBuffer(animationComp->SPW_VertexMap);
                     }
                 }
            });
}

void SPW::AnimationSystem::onUpdate(TimeDuration dt)
{
    //TICKTOCK;
    double deltaTime = dt.toSecond();
    ComponentGroup<SPW::AnimationComponent,SPW::IDComponent,SPW::MeshComponent> animatedGroup;
    locatedScene.lock()->forEachEntityInGroup
    (animatedGroup,
     [this,&animatedGroup,deltaTime](const Entity &entity){

         AnimatedEntity animatedEntity = entity.combinedInGroup(animatedGroup);

         auto animationComp = entity.component<SPW::AnimationComponent>();
         auto modelComp = entity.component<SPW::MeshComponent>();

         if (animationComp->onGoingAnim)
         {
             animationComp->onGoingAnim->update(deltaTime);
         }

         animationComp->SPW_AnimSSBO->updateDynamicBuffer(animationComp->onGoingAnim);
         animationComp->SPW_AnimSSBO->pushData(modelComp);
     });
}

void SPW::AnimationSystem::afterUpdate() {}
void SPW::AnimationSystem::onStop() {}



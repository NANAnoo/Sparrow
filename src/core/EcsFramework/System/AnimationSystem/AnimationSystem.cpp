//
// Created by Shawwy on 2/24/2023.
//

#include "AnimationSystem.h"


void SPW::AnimationSystem::initial()
{

}
void SPW::AnimationSystem::beforeUpdate()
{

}

void SPW::AnimationSystem::onUpdate(TimeDuration dt)
{
    //Update the Animation component
    double deltaTime = dt.toSecond();

    //To be implemented, choose an animation based on a name;
    //IDComponent: Identify entity with A specific ID
    //ModelComponent: Change the model of that entity
    //AnimationComponent: return a transFormMatrix

    std::string animationName = "Walk";
    ComponentGroup<SPW::AnimationComponent,SPW::IDComponent,SPW::ModelComponent> animatedGroup;

    locatedScene.lock()->forEachEntityInGroup
    (animatedGroup,
     [this,&animatedGroup,&animationName,&deltaTime](const Entity &entity){

         AnimatedEntity animatedEntity = entity.combinedInGroup(animatedGroup);

         auto animationComp = entity.component<SPW::AnimationComponent>();

         if (animationComp->bStarted && !animationComp->bStop) //Should receive a name of an animation
         {
             this->updateAnimation(animationName,deltaTime,*std::get<0>(animatedEntity));
             this->updateModel();
         }else{
             this->playAnimation(animationName,deltaTime,*std::get<0>(animatedEntity));
         }

         //Stop playing animation, how to get stop signal?
         if (animationComp->bStop && animationComp->bStarted)
         {
             //Stop playing animation
             this->stopAnimation(*animationComp);
         }
    });
}

void SPW::AnimationSystem::playAnimation(std::string name, float dt, AnimationComponent &animationComponent)
{
    std::weak_ptr<AnimationClip> temp = findAnimation(name,animationComponent);
    if (temp.expired())
        return;
    else
    {
        animationComponent.currentAnimation = temp.lock();
        animationComponent.currentTime = 0.0f;
        animationComponent.bStarted = true;

        temp.reset();
        updateAnimation(name,dt,animationComponent);
    }
}

std::shared_ptr<AnimationClip> SPW::AnimationSystem::findAnimation(std::string name,AnimationComponent& animationComponent)
{
    for(std::weak_ptr<AnimationClip> n : animationComponent.skeleton.m_animClips)
    {
        if (n.lock()->name == name)
            return n.lock();
    }
    return nullptr;
}

void SPW::AnimationSystem::updateAnimation(std::string name, float dt, AnimationComponent &animationComponent)
{
    if (!animationComponent.currentAnimation.expired())
    {
        float currentTime = animationComponent.currentTime;

        currentTime += dt;

        if (currentTime >= animationComponent.currentAnimation.lock()->duration)
        {
            animationComponent.currentTime = 0.0f;
        }
        animationComponent.currentTime = currentTime;

        calculateBoneTransform(findRootNode(animationComponent),
                               glm::mat4(1.0f),
                               animationComponent,
                               currentTime);
    }
}

std::shared_ptr<BoneInfo> SPW::AnimationSystem::findRootNode(AnimationComponent &animationComponent)
{
    for(std::weak_ptr<BoneInfo> bone : animationComponent.skeleton.m_Bones)
    {
        if (bone.lock()->parentID == -1)
            return bone.lock();
    }
    return nullptr;
}

void SPW::AnimationSystem::calculateBoneTransform(std::shared_ptr <BoneInfo> bone,
                                                  glm::mat4 parrentTransform,
                                                  AnimationComponent& animationComponent,
                                                  float currentTime)
{
    std::string boneName = bone->name;
    glm::mat4 localTransform = glm::mat4(1.0);
    glm::mat4 boneOffset = glm::mat4(1.0);

    //TODO: Verify name of a bone and the name of a animNode
    AnimationNode currentNode = findAnimationNode(boneName,animationComponent.currentAnimation);

    if (currentNode.nodeName == "")
        return;
    else
    {
        localTransform = getUpdatedTransform(currentNode,currentTime);
        boneOffset = bone->offsetMatrix;

        glm::mat4 finalTransfrom = parrentTransform * localTransform * boneOffset;

        //TODO: Index of Boneinfo[] is same as AnimNode[]?
        animationComponent.finalBoneMatrices[bone->boneID] = finalTransfrom;

        for(int i = 0; i < bone->childrenIDs.size(); i++)
            calculateBoneTransform(animationComponent.skeleton.m_Bones[i],finalTransfrom,animationComponent,currentTime);
    }
}


float getScaling(float lastTimeStamp,float nextTimeStamp,float currentTime)
{
    float factor = 0.0f;
    float midLength = currentTime - lastTimeStamp;
    float frameLength = nextTimeStamp = lastTimeStamp;
    factor = midLength/frameLength;
    return factor;
}

glm::mat4 SPW::AnimationSystem::getUpdatedTransform(AnimationNode node,float currentTime)
{

    if (node.keyFrames.size() == 1)
    {
        KeyFrame keyFrame = node.keyFrames[0];
        glm::mat4 transform;
        return transform;
    }

    int index;
    int nextIndex;
    glm::mat4 transform;
    //Get transform here
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scaling;
    for (int i = 0; i < node.keyFrames.size()-1; ++i)
    {
        if (currentTime < node.keyFrames[i+1].time)
        {
            index = i-1;
            nextIndex = index+1;
        }
    }
    float factor = getScaling(node.keyFrames[index].time,node.keyFrames[nextIndex].time,currentTime);

    position = glm::mix(node.keyFrames[index].position,node.keyFrames[nextIndex].position,factor);
    rotation = glm::mix(node.keyFrames[index].rotation,node.keyFrames[nextIndex].rotation,factor);
    scaling = glm::mix(node.keyFrames[index].sacling,node.keyFrames[nextIndex].sacling,factor);

    //TODO:Vec3->mat4  quat->mat4
    //Missing glm::vec3 -> glm::mat4 transfrom
    //transform = glm::translate(glm::mat4(1,0),position)
    return transform;
}

void SPW::AnimationSystem::stopAnimation(AnimationComponent& animationComponent)
{
    animationComponent.currentTime = 0.0f;
}


AnimationNode SPW::AnimationSystem::findAnimationNode(std::string name,std::weak_ptr<AnimationClip> currentAnimation)
{
    AnimationNode result;
    for(AnimationNode temp : currentAnimation.lock()->nodeAnimations)
    {
        if (temp.nodeName == name)
            result = temp;
    }
    return result;
}

void SPW::AnimationSystem::updateModel() {}
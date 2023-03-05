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

    double deltaTime = dt.toSecond();

    ComponentGroup<SPW::AnimationComponent,SPW::IDComponent,SPW::ModelComponent> animatedGroup;

    locatedScene.lock()->forEachEntityInGroup
    (animatedGroup,
     [this,&animatedGroup,&deltaTime](const Entity &entity){

         AnimatedEntity animatedEntity = entity.combinedInGroup(animatedGroup);

         //Get animationComp and modelComp of this entity
         auto animationComp = entity.component<SPW::AnimationComponent>();
         auto modelComp = entity.component<SPW::ModelComponent>();

         //See if animation component has been initialized
         //TODO:Perhaps this is already done when the animation component is created?
         if (!animationComp->bInitialized)
            initializeComponent(*animationComp,*modelComp);
        else
        {

            //If we need to Switch animation?
            //TODO:Do we get the name in modelComp? std::string animationName = modelComp->currentAnim
            std::string animationName = "Walk";
            if (animationName.compare(animationName))
            {
                this->playAnimation(animationName,deltaTime,*animationComp);

            }else{

                //If this animation is played
                //TODO:These bool could be changed in event responding system
                if (animationComp->bStarted && !animationComp->bStop)
                {
                    //Get name: std::string animName = modelComp->animName ?
                    this->updateAnimation(animationName,deltaTime,*std::get<0>(animatedEntity));

                    //TODO: Will model be changed in this stage? or it should be done in rendering stage
                    this->updateModel();

                //If we start an animation from stop state
                }else{
                    this->playAnimation(animationName,deltaTime,*std::get<0>(animatedEntity));
                }
            }


            //TODO:These bool could be changed in event responding system
            if (animationComp->bStop && !animationComp->bStarted)
            {
                //Stop playing animation
                this->stopAnimation(*animationComp);
            }
        }
    });
}

void SPW::AnimationSystem::initializeComponent(AnimationComponent &animationComponent,ModelComponent& modelComponent)
{
    if(animationComponent.skeleton != nullptr && animationComponent.skeleton->m_animClips.size() != 0)
    {
        animationComponent.finalBoneMatrices.reserve(animationComponent.skeleton->m_Bones.size());
        vertexBoneMap(animationComponent,modelComponent);
        animationComponent.bInitialized = true;
    }
}

void SPW::AnimationSystem::vertexBoneMap(AnimationComponent &animationComponent,ModelComponent& modelComponent)
{

    //Get number of vertices
    std::weak_ptr<Model> model = modelComponent.model;
    unsigned int numVertex = 0;
    for(std::weak_ptr<Mesh> mesh : model.lock()->GetMeshes())
    {
        numVertex+=mesh.lock()->vertices.size();
    }

    animationComponent.verMapBone.reserve(numVertex);

    for(std::weak_ptr<BoneInfo> boneInfo : animationComponent.skeleton->m_Bones)
    {
        for(Weight weight : boneInfo.lock()->weights)
        {
            uint32_t vertexID = weight.vertexID;
            uint32_t boneID = boneInfo.lock()->boneID;
            float value = weight.value;

            animationComponent.verMapBone[vertexID].boneID.push_back(boneID);
            animationComponent.verMapBone[vertexID].weight.push_back(value);
        }
    }
}



void SPW::AnimationSystem::playAnimation(std::string name, float dt, AnimationComponent &animationComponent)
{
    std::weak_ptr<AnimationClip> temp = findAnimation(name,animationComponent);
    if (temp.expired())
        return;
    else
    {
        //Start an animation from stop state
        if (animationComponent.currentAnimation.expired())
        {
            animationComponent.currentAnimation = temp.lock();
            animationComponent.currentTime = 0.0f;
        }
        //Start a new animation, we stop the current one and start a new one
        else
        {
            stopAnimation(animationComponent);
            animationComponent.currentAnimation = temp.lock();
        }

        animationComponent.bStarted = true;
        temp.reset();
        updateAnimation(name,dt,animationComponent);
    }
}

std::shared_ptr<AnimationClip> SPW::AnimationSystem::findAnimation(std::string name,AnimationComponent& animationComponent)
{
    for(std::weak_ptr<AnimationClip> n : animationComponent.skeleton->m_animClips)
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
    for(std::weak_ptr<BoneInfo> bone : animationComponent.skeleton->m_Bones)
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

    if (bone == nullptr)
        return;

    std::string boneName = bone->name;
    glm::mat4 localTransform = glm::mat4(1.0);
    glm::mat4 boneOffset = glm::mat4(1.0);

    AnimationNode currentNode = findAnimationNode(boneName,animationComponent.currentAnimation);

    if (currentNode.nodeName == "")
        return;
    else
    {
        localTransform = getUpdatedTransform(currentNode,currentTime);
        boneOffset = bone->offsetMatrix;

        glm::mat4 finalTransfrom = parrentTransform * localTransform * boneOffset;

        animationComponent.finalBoneMatrices[bone->boneID] = finalTransfrom;

        for(int i = 0; i < bone->childrenIDs.size(); i++)
            calculateBoneTransform(animationComponent.skeleton->m_Bones[i],finalTransfrom,animationComponent,currentTime);
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

    rotation = glm::normalize(glm::slerp(node.keyFrames[index].rotation,node.keyFrames[nextIndex].rotation,factor));

    scaling = glm::mix(node.keyFrames[index].sacling,node.keyFrames[nextIndex].sacling,factor);

    //Missing glm::vec3 -> glm::mat4 transfrom
    transform = glm::translate(glm::mat4(1.0f),position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f),scaling);

    return transform;
}

void SPW::AnimationSystem::stopAnimation(AnimationComponent& animationComponent)
{
    animationComponent.currentTime = 0.0f;
    animationComponent.currentAnimation.reset();
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
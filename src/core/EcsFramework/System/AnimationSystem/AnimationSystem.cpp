//
// Created by Shawwy on 2/24/2023.
//

#include "AnimationSystem.h"


void SPW::AnimationSystem::initial()
{

}
void SPW::AnimationSystem::beforeUpdate()
{
    //TODO: 逻辑转变：所有的animation clip进行关键帧的transform计算，只处理一次
    //TODO: 逻辑转变：animation system 每次更新只返回权重（根据目标动画的关键帧time stamp，计算到插值权重）

    //TODO：逻辑基础（每个顶点对应的骨骼数量不同，因此每个顶点的weight[]数组长度不同，
    // 当把所有顶点的weight都拼在一起的时候，
    // 1：需要startIndex[]数组，记录每个顶点的开始索引
    // 2：需要weight[]数组将所有顶点的所有对应
    // 3：需要boneIndex[]数组记录对应骨骼的index (骨骼index匹配boneInfo数组的index）
    // 4: 需要 int[2]记录具体哪两关键帧，float[2]记录该两帧对应的线性插值权重
    // 5: b(帧数) * a(骨骼数量) * [4*4](矩阵大小) 的已经提前算好的关键帧transform
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
         if (!animationComp->bInitialized)
            initializeComponent(*animationComp,*modelComp);
        else
        {

            //If we need to Switch animation?
            std::string animationName = animationComp->incomingAnimName;
            if (animationName.compare(animationName))
            {
                this->playAnimation(animationName,deltaTime,*animationComp);

            }else{

                //If this animation is played
                if (animationComp->state == SPW::State::started)
                {
                    //Get name: std::string animName = modelComp->animName ?
                    this->updateAnimation(animationName,deltaTime,*std::get<0>(animatedEntity));

                    this->updateModel();

                //If we start an animation from stop state
                }else{
                    this->playAnimation(animationName,deltaTime,*std::get<0>(animatedEntity));
                }
            }

            if (animationComp->state == SPW::State::stopped)
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

        //TODO:Precalculate all transform
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

        animationComponent.state = SPW::State::started;
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

        //Flatten the matrices
        if (animationComponent.finalBoneMatrices.size() != 0)
        {
            for (auto matrix : animationComponent.finalBoneMatrices)
            {
                animationComponent.finalBoneArray.push_back(glm::value_ptr(matrix));
            }
        }
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
    //TODO: Performance optimize
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
    animationComponent.finalBoneMatrices.clear();
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
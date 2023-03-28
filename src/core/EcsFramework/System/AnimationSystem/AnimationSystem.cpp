
// Created by Shawwy on 2/24/2023.
//

#include "AnimationSystem.h"


void SPW::AnimationSystem::initial()
{

}
void SPW::AnimationSystem::beforeUpdate()
{
    //TODO: 逻辑转变：所有的animation clip进行关键帧的transform计算，只处理一次 (done)
    //TODO: 逻辑转变：animation system 每次更新只返回权重（根据目标动画的关键帧time stamp，计算到插值权重）

    //TODO：逻辑基础（每个顶点对应的骨骼数量不同，因此每个顶点的weight[]数组长度不同，
    // 当把所有顶点的weight都拼在一起的时候，
    // 1：需要startIndex[]数组，记录每个顶点的开始索引 done
    // 2：需要weight[]数组将所有顶点的所有对应 done
    // 3：需要boneIndex[]数组记录对应骨骼的index (骨骼index匹配boneInfo数组的index）done
    // 4: 需要size[]数组记录每个顶点关联骨头节点的数量 done
    // 5: b(帧数) * a(骨骼数量) * [4*4](矩阵大小) 的已经提前算好的关键帧transform (done)
    // ===========================
    // 6: 需要 int[2]记录具体哪两关键帧，float[2]记录该两帧对应的线性插值权重 (done [animationComponent中拿到] )


    ComponentGroup<SPW::AnimationComponent,SPW::IDComponent,SPW::ModelComponent> animatedGroup;

    locatedScene.lock()->forEachEntityInGroup
            (animatedGroup,
             [this,&animatedGroup](const Entity &entity)
             {

                 AnimatedEntity animatedEntity = entity.combinedInGroup(animatedGroup);

                 //Get animationComp and modelComp of this entity
                 auto animationComp = entity.component<SPW::AnimationComponent>();
                 auto modelComp = entity.component<SPW::ModelComponent>();

                 if (!animationComp->bLoaded)
                 {

                     //precalculateTransform(*animationComp);
                     initializeComponent(*animationComp,*modelComp);

                     animationComp->bLoaded = true;

                     animationComp->current_clip.starts = std::make_shared<StorageBuffer>(
                             "WeightDictStart",
                             animationComp->bufferInfo.startIndex.size() * sizeof(int),
                             0);
                     animationComp->current_clip.starts->updateSubData(
                             animationComp->bufferInfo.startIndex.data(),
                             0,
                             animationComp->bufferInfo.startIndex.size() * sizeof(int));


                     animationComp->current_clip.sizes = std::make_shared<StorageBuffer>(
                             "WeightDictSize",
                             animationComp->bufferInfo.size.size() * sizeof(int),
                             1);
                     animationComp->current_clip.sizes->updateSubData(
                             animationComp->bufferInfo.size.data(),
                             0,
                             animationComp->bufferInfo.size.size() * sizeof(int));


                     animationComp->current_clip.boneIndices = std::make_shared<StorageBuffer>(
                             "WeightDictKey",
                             animationComp->bufferInfo.boneID.size() * sizeof(int),
                             2);
                     animationComp->current_clip.boneIndices->updateSubData(
                             animationComp->bufferInfo.boneID.data(),
                             0,
                             animationComp->bufferInfo.boneID.size() * sizeof(int));


                     animationComp->current_clip.weights = std::make_shared<StorageBuffer>(
                             "WeightDictValue",
                             animationComp->bufferInfo.weights.size() * sizeof(float),
                             3);
                     animationComp->current_clip.weights->updateSubData(
                             animationComp->bufferInfo.weights.data(),
                             0,
                             animationComp->bufferInfo.weights.size() * sizeof(float));


                     animationComp->current_clip.mats = std::make_shared<StorageBuffer>(
                             "WeightMatries",
                             animationComp->flattenTransform.size() * sizeof(glm::mat4),
                             4);





                     modelComp->preRenderCommands.pushCommand(
                             RenderCommand(&RenderBackEndI::initStorageBuffer,
                                           animationComp->current_clip.starts));
                     modelComp->preRenderCommands.pushCommand(
                             RenderCommand(&RenderBackEndI::initStorageBuffer,
                                           animationComp->current_clip.sizes));
                     modelComp->preRenderCommands.pushCommand(
                             RenderCommand(&RenderBackEndI::initStorageBuffer,
                                           animationComp->current_clip.boneIndices));
                     modelComp->preRenderCommands.pushCommand(
                             RenderCommand(&RenderBackEndI::initStorageBuffer,
                                           animationComp->current_clip.weights));
                     modelComp->preRenderCommands.pushCommand(
                             RenderCommand(&RenderBackEndI::initStorageBuffer,
                                           animationComp->current_clip.mats));
                 }
            });
}
static int i = 0;
void SPW::AnimationSystem::onUpdate(TimeDuration dt)
{

    double deltaTime = dt.toSecond();

    ComponentGroup<SPW::AnimationComponent,SPW::IDComponent,SPW::ModelComponent> animatedGroup;

    locatedScene.lock()->forEachEntityInGroup
    (animatedGroup,
     [this,&animatedGroup,&deltaTime](const Entity &entity){

         AnimatedEntity animatedEntity = entity.combinedInGroup(animatedGroup);

         auto animationComp = entity.component<SPW::AnimationComponent>();
         auto modelComp = entity.component<SPW::ModelComponent>();

         animationComp->current_clip.mats->updateSubData(
                 animationComp->flattenTransform.data(),
                 0,
                 animationComp->flattenTransform.size() * sizeof(glm::mat4)
         );

         modelComp->pipeLineCommands.pushCommand(
                 RenderCommand(&Shader::setStorageBuffer, animationComp->current_clip.starts)
         );
         modelComp->pipeLineCommands.pushCommand(
                 RenderCommand(&Shader::setStorageBuffer, animationComp->current_clip.sizes)
         );
         modelComp->pipeLineCommands.pushCommand(
                 RenderCommand(&Shader::setStorageBuffer, animationComp->current_clip.boneIndices)
         );
         modelComp->pipeLineCommands.pushCommand(
                 RenderCommand(&Shader::setStorageBuffer, animationComp->current_clip.weights)
         );
         modelComp->pipeLineCommands.pushCommand(
                 RenderCommand(&Shader::setStorageBuffer, animationComp->current_clip.mats)
         );
//         modelComp->pipeLineCommands.pushCommand(
//                 RenderCommand(&Shader::SetUniformValue<int>, std::string("currentFrame"), i)
//         );
//         i =  (i + 1) % 60;

         if (!animationComp->bInitialized)
            initializeComponent(*animationComp,*modelComp);
        else
        {

            std::string animationName = animationComp->incomingAnimName;
            if (!animationName.compare(animationName))
            {
                this->playAnimation(animationName,deltaTime,*animationComp);

            }else{

                if (animationComp->state == SPW::State::started)
                {
                    this->updateAnimation(animationName,deltaTime,*animationComp);

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
        //loading data
    });
}
float getScaling(float lastTimeStamp,float nextTimeStamp,float currentTime)
{
    float factor = 0.0f;
    float midLength = currentTime - lastTimeStamp;
    float frameLength = nextTimeStamp - lastTimeStamp;
    factor = midLength/frameLength;
    return factor;
}



void SPW::AnimationSystem::initializeComponent(AnimationComponent &animationComponent,ModelComponent& modelComponent)
{
    if(animationComponent.skeleton != nullptr && animationComponent.skeleton->m_animClips.size() != 0 && animationComponent.bInitialized==false)
    {

        //animationComponent.finalBoneMatrices.reserve(animationComponent.skeleton->m_Bones.size());

        changeMap(animationComponent, modelComponent);

        animationComponent.bInitialized = true;
    }
}

void SPW::AnimationSystem::changeMap(AnimationComponent &animationComponent, ModelComponent& modelComponent)
{

    //Get number of vertices
    std::weak_ptr<Model> model = modelComponent.model;
    unsigned int numVertex = 0;
    std::vector<VerMapBone> verMapBone;
    for(std::weak_ptr<Mesh> mesh : model.lock()->GetMeshes())
    {
        numVertex+=mesh.lock()->vertices.size();
    }

    verMapBone.resize(numVertex);

    for(std::weak_ptr<BoneInfo> boneInfo : animationComponent.skeleton->m_Bones)
    {
        for(Weight weight : boneInfo.lock()->weights)
        {
            uint32_t vertexID = weight.vertexID;
            uint32_t boneID = boneInfo.lock()->boneID;
            float value = weight.value;

            verMapBone[vertexID].boneID.push_back(boneID);
            verMapBone[vertexID].weight.push_back(value);
        }
    }
    vertexBoneMapping(animationComponent, modelComponent, verMapBone);
}

void SPW::AnimationSystem::vertexBoneMapping(AnimationComponent &animationComponent, ModelComponent &modelComponent,
                                             std::vector<VerMapBone> verMapBone)
{
    //Get number of vertices
    animationComponent.bufferInfo.startIndex.reserve(verMapBone.size());

    //For every vertex
    int startIndex = 0;
    for (int i = 0; i < verMapBone.size(); ++i)
    {

        auto temp = verMapBone[i];
        animationComponent.bufferInfo.startIndex.push_back(startIndex);

        for (int j = 0; j < temp.boneID.size(); j++)
        {
            animationComponent.bufferInfo.boneID.push_back(temp.boneID[j]);
            animationComponent.bufferInfo.weights.push_back(temp.weight[j]);
        }
        startIndex += temp.boneID.size();
        animationComponent.bufferInfo.size.push_back(temp.boneID.size());
    }
}


void SPW::AnimationSystem::playAnimation(std::string name, float dt, AnimationComponent &animationComponent)
{
    std::weak_ptr<AnimationClip> temp = findAnimation(name,animationComponent);
    animationComponent.flattenTransform.resize(animationComponent.skeleton->m_Bones.size());
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
        //updateFramesWeight(name,dt,animationComponent);
        updateAnimation(name,dt,animationComponent);
    }
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

        calculateBoneTransform(&(animationComponent.skeleton->m_assimpRootNode),
                               glm::mat4(1.0f),
                               animationComponent,
                               currentTime);
    }
}
glm::mat4 SPW::AnimationSystem::getUpdatedTransform(AnimationNode* node,float currentTime)
{

    if (node->keyFrames.size() == 1)
    {
        KeyFrame keyFrame = node->keyFrames[0];
        glm::mat4 transform;
        return transform;
    }

    int index = 0;
    int nextIndex = 0;
    glm::mat4 transform;
    //Get transform here
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scaling;
    for (int i = 0; i < node->keyFrames.size()-1; i++)
    {
        if (currentTime < node->keyFrames[i+1].time)
        {
            index = i;
            nextIndex = index+1;
            break;
        }
    }


    //All keyframes are at 0 second and won't change
    if (node->keyFrames[nextIndex].time - node->keyFrames[index].time < 1e-6)
    {

        position = node->keyFrames[index].position;
        rotation = glm::normalize(node->keyFrames[index].rotation);
        scaling = node->keyFrames[index].sacling;

        glm::mat4 pos_translate = glm::translate(glm::mat4(1.0f),position);
        glm::mat4 rot_cast = glm::mat4_cast(rotation);
        glm::mat4 scal_scale = glm::scale(glm::mat4(1.0f),scaling);

        transform = pos_translate * rot_cast * scal_scale;

    }else
    {
        float factor = getScaling(node->keyFrames[index].time,node->keyFrames[nextIndex].time,currentTime);

        position = glm::mix(node->keyFrames[index].position,node->keyFrames[nextIndex].position,factor);

        rotation = glm::normalize(glm::slerp(node->keyFrames[index].rotation,node->keyFrames[nextIndex].rotation,factor));

        scaling = glm::mix(node->keyFrames[index].sacling,node->keyFrames[nextIndex].sacling,factor);

        //Missing glm::vec3 -> glm::mat4 transfrom
        transform = glm::translate(glm::mat4(1.0f),position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f),scaling);
    }

    return transform;
}



std::shared_ptr<SPW::BoneInfo> findBoneInSkeleton(SPW::AnimationComponent& animationComponent,std::string animNodeName)
{

    for(auto bone : animationComponent.skeleton->m_Bones)
    {
        if (bone->name == animNodeName)
            return bone;
    }
    return nullptr;
}


void SPW::AnimationSystem::calculateBoneTransform(const AssimpNodeData* node,
                                                  glm::mat4 parrentTransform,
                                                  AnimationComponent& animationComponent,
                                                  float currentTime)
{

    //TODO: Here, the node is aiNode
    std::string assimpName = node->name; //get name from assimp Node
    glm::mat4 localTransform = node->transformation; // get localTransform from assimpNode

    //Check if this assimpName has engaged in this animation
    AnimationNode currentNode = findAnimationNode(assimpName,animationComponent.currentAnimation);

    if (currentNode.nodeName != "")
    {
        localTransform = getUpdatedTransform(&currentNode,currentTime);
    }

    localTransform = parrentTransform * localTransform;
    //Check if this assimpNode has a corresponding bone in skeleton
    std::shared_ptr<SPW::BoneInfo> temp = findBoneInSkeleton(animationComponent,assimpName);

    if (temp)
    {
        uint32_t boneId = temp->boneID;
        auto boneOffset = temp->offsetMatrix;
        animationComponent.flattenTransform[boneId] = localTransform * boneOffset;
    }

    for(int i = 0; i < node->childrenCount; i++)
    {
        calculateBoneTransform(&node->children[i],localTransform,animationComponent,currentTime);
    }

}


std::shared_ptr<SPW::AnimationClip> SPW::AnimationSystem::findAnimation(std::string name,AnimationComponent& animationComponent)
{
    for(std::weak_ptr<AnimationClip> n : animationComponent.skeleton->m_animClips)
    {
        if (n.lock()->name == name)
            return n.lock();
    }
    return nullptr;
}

void SPW::AnimationSystem::stopAnimation(AnimationComponent& animationComponent)
{
    animationComponent.currentTime = 0.0f;
    animationComponent.currentAnimation.reset();
}

SPW::AnimationNode SPW::AnimationSystem::findAnimationNode(std::string name,std::weak_ptr<AnimationClip> currentAnimation)
{
    AnimationNode result;
    for(AnimationNode temp : currentAnimation.lock()->nodeAnimations)
    {
        if (temp.nodeName == name)
        {
            result = temp;
            return result;
        }

    }
    return result;
}


//===========================================================================================================


void SPW::AnimationSystem::afterUpdate() {}
void SPW::AnimationSystem::onStop() {}

//void SPW::AnimationSystem::calcuKeyframeTransform(std::shared_ptr<BoneInfo> bone, glm::mat4 parrentTransform,
//                                                  AnimationComponent &animationComponent, uint32_t frameIndex,uint32_t boneIndex,
//                                                  std::weak_ptr<AnimationClip> Animclip)
//{
//
//    //TODO: Performance optimize
//    if (bone == nullptr)
//        return;
//
//    std::string boneName = bone->name;
//    glm::mat4 localTransform = glm::mat4(1.0);
//    glm::mat4 boneOffset = glm::mat4(1.0);
//
//    AnimationNode currentNode = findAnimationNode(boneName,Animclip);
//
//    //Find root node.
//    //找不到名字不代表是根节点
//    if (currentNode.nodeName == "")
//    {
//        glm::mat4 finalTransfrom = glm::mat4(1.0f);
//        for (int i = 0; i < bone->childrenIDs.size(); i++)
//        {
//            std::int32_t id = bone->childrenIDs[i];
//            calcuKeyframeTransform(animationComponent.skeleton->m_Bones[id], finalTransfrom, animationComponent, frameIndex, id,
//                                   Animclip);
//        }
//
//    }else
//    {
//
//        //if (m_RootNodes bone is engaged in this animation by finding it in m_Bones array of Animation)
//        // AnimNode对应的骨骼（这里指所有骨骼，包含了不影响顶点的骨骼）未必在skeleton里面存在，因此如果不存在，则使用默认transform
//        //localTransform = animNode->transformation
//
//        //如果存在，则计算关键帧
//        //else, (the local transform is getKeyFrameTransform)
//        //localTransform = getKeyframeTransform(currentNode, frameIndex);
//
//        //globalTransformation = parentTransform * localTransform;
//
//        //如果这个动画对应的骨骼是
//        //m_BoneInfoMap : all bones  但是动画未必调用到全部的骨骼
//        //If this bone is affecting vertices, then we set its finalBoneMatrices
//        //If not, the globalTransformation is passed as parrent transform
//
//        glm::mat4 finalTransfrom = glm::mat4 (1.0f);
//        glm::mat4 testTransform = parrentTransform;
//
//        localTransform = getKeyframeTransform(currentNode, frameIndex);
//        boneOffset = bone->offsetMatrix;
//
//        finalTransfrom = parrentTransform * localTransform * boneOffset;
//        animationComponent.keyframeMatrice[frameIndex][boneIndex] = finalTransfrom;
//
//        for(int i = 0; i < bone->childrenIDs.size(); i++)
//        {
//            std::int32_t id = bone->childrenIDs[i];
//            calcuKeyframeTransform(animationComponent.skeleton->m_Bones[id], finalTransfrom, animationComponent, frameIndex,id,Animclip);
//        }
//    }
//
//}
//void SPW::AnimationSystem::updateFramesWeight(std::string name, float dt, AnimationComponent &animationComponent)
//{
//    if (!animationComponent.currentAnimation.expired()) {
//        float currentTime = animationComponent.currentTime;
//
//        currentTime += dt;
//
//        if (currentTime >= animationComponent.currentAnimation.lock()->duration) {
//            animationComponent.currentTime = 0.0f;
//        }
//
//        animationComponent.currentTime = currentTime;
//
//        //Get current animation
//        std::weak_ptr<AnimationClip> clip = animationComponent.currentAnimation.lock();
//        //Calculate weights for each transformation of these 2 keyframes
//        int preFrame;
//        int postFrame;
//        float preFactor;
//        float postFactor;
//        for (int i = 0; i < clip.lock()->nodeAnimations[1].keyFrames.size(); i++)
//        {
//            if (clip.lock()->nodeAnimations[1].keyFrames[i+1].time > currentTime)
//            {
//                preFrame = i;
//                postFrame = i+1;
//
//                float preTime = clip.lock()->nodeAnimations[1].keyFrames[preFrame].time;
//                float postTime = clip.lock()->nodeAnimations[1].keyFrames[postFrame].time;
//
//                preFactor = getScaling(preTime,postTime,currentTime);
//                postFactor = 1.0-preFactor;
//            }
//        }
//        animationComponent.indices[0] = preFrame;
//        animationComponent.indices[1] = postFrame;
//        animationComponent.frameWeights[0] = preFactor;
//        animationComponent.frameWeights[1] = postFactor;
//    }
//}
//void SPW::AnimationSystem::precalculateTransform(AnimationComponent &animationComponent)
//{
//    using ClipTransform = std::vector<std::vector<glm::mat4>>;
//    //Get root node
//    std::weak_ptr<BoneInfo> rootNode = findRootNode(animationComponent);
//
//    //Calculate each animCli to get animNum of b(frame counts) * a(num_bones) * [4*4](matrix size) transform matrices
//    for(std::weak_ptr<AnimationClip> clip : animationComponent.skeleton->m_animClips)
//    {
//        animationComponent.keyframeMatrice.resize(clip.lock()->frameCount);
//
//        //对每一帧进行计算，返回 a(骨骼数量) * [4*4](矩阵大小)
//        for (uint32_t i = 0; i < clip.lock()->frameCount; i++)
//        {
//            animationComponent.keyframeMatrice[i].resize(animationComponent.skeleton->m_Bones.size(),glm::mat4(1.0f));
//
//            glm::mat4 rootTransfrom = glm::mat4(1.0f);
//            calcuKeyframeTransform(rootNode.lock(), rootTransfrom, animationComponent, i, 0,
//                                   clip);
//
//        }
//
//        std::string animName = clip.lock()->name;
//
//        ClipTransform keyframeMatrice = animationComponent.keyframeMatrice;
//
//        AnimationClipTransform animationClipTransform;
//        animationClipTransform.finalKeyframeMatrices=keyframeMatrice;
//        animationClipTransform.animName=animName;
//
//        //Flatten the matrices
//        for(auto boneClip : keyframeMatrice)
//        {
//            for(int i = 0 ; i < boneClip.size();i++)
//            {
//                animationClipTransform.flattenTransform.push_back(boneClip[i]);
//            }
//        }
//
//        animationClipTransform.frameCount = clip.lock()->frameCount;
//
//        animationComponent.finalKeyMatricesAllClips.emplace_back(animationClipTransform);
//    }
//
//}
//glm::mat4 SPW::AnimationSystem::getKeyframeTransform(AnimationNode node,int index)
//{
//    if (node.keyFrames.size() == 1)
//    {
//        KeyFrame keyFrame = node.keyFrames[0];
//        glm::mat4 transform;
//        return transform;
//    }
//
//    glm::mat4 transform = glm::mat4(1.0f);
//    //Get transform here
//    if (index < node.keyFrames.size())
//    {
//        glm::vec3 position;
//        glm::quat rotation;
//        glm::vec3 scaling;
//
//        position = node.keyFrames[index].position;
//
//        rotation = node.keyFrames[index].rotation;
//
//        scaling = node.keyFrames[index].sacling;
//
//        //Missing glm::vec3 -> glm::mat4 transfrom
//        transform = glm::translate(glm::mat4(1.0f),position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f),scaling);
//    }
//    return transform;
//}

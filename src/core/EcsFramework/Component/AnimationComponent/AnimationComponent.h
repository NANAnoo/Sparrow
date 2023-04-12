//
// Created by Shawwy on 2/24/2023.
//



#ifndef SPARROW_ANIMATIONCOMPONENT_H
#define SPARROW_ANIMATIONCOMPONENT_H

#include "../ComponentI.h"
#include "EcsFramework/Component/MeshComponent.hpp"
#include "Model/Animation/Skeleton.h"
#include "Model/Animation/AnimDefinitions.h"
#include "Model/Model.h"
#include "Model/Mesh.h"
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include "glm/glm.hpp"
#include "Render/StorageBuffer.h"
#include "Utils/ThreadPool.h"
#include <future>
#include <glm/gtx/matrix_decompose.hpp>
//#include "Animation.h"
namespace SPW {

    //Start: Start an animation from 'Stopped' state or 'Paused' state
    //Reset the current time and change to OnPlaying
    //Stopped: Animation is stopped at time: 0
    //OnPlaying -> stop: bUpdate = false; (AnimComponent to                           animation)
    //Paused: Animation is stopped during playing
    //Onplaying: Animation is playing and keeps updating

    //Begin a new Animation
    //1:
    enum class AnimationState {Start,Stopped,Paused,OnPlaying};
    enum class AnimationAction {Reset,Swap,Pause,Resume};

    struct VerMapBone
    {
        //Vertex -> <BoneID> -> (stores Index of finalBoneMatrix)
        std::vector<uint32_t> boneID;
        //Weight -> <double) -> (stores corresponding weight; (boneID[0] - Weight[0]) )
        std::vector<float> weight;
    };

    struct MatrixPerFrame
    {
        MatrixPerFrame(int boneSize,float timeStamp)
        {
            transformMatrix.resize(boneSize,glm::mat4(1.0f));
            this->timeStamp = timeStamp;
        }

        MatrixPerFrame() = default;

        std::vector<glm::mat4> transformMatrix;
        float timeStamp;
    };


    class SPWAnimationDelegateI{
    public:
        virtual std::weak_ptr<AnimationClip> playAnim(const std::string &name, bool isLoop);
    };


    //SPW::Animation class
    //包装Animation类，内含骨骼
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
            state = AnimationState::Stopped;
            isLoop = false;
            bUpdate = false;
            isPreLoaded = false;
            m_assimpRootNode = skeleton->m_assimpRootNode;
            NameMappingBones(skeleton->m_Bones);
            finalBoneMatrices.resize(m_BoneMap.size(),glm::mat4(1.0f));
            preload();
        }

        void preload() {
            // totol frames Get total frames
            // 多线程计算总关键帧的所有finalBoneMatrices
            // calculate用于在某两帧之间进行mix // 60帧
            //Initialize matrices
            preload_finalBoneMatrices.resize(120);
            for(int i = 0 ; i < 120; ++i)
            {
                float index = i * 1.0f;
                float timeStamp = (duration / 119.0f) * index;
                preload_finalBoneMatrices[i].timeStamp = timeStamp;
                preload_finalBoneMatrices[i].transformMatrix.resize(m_BoneMap.size(),glm::mat4(1.0f));
            }

            ThreadPool threadPool(3,16);
            std::vector<std::future<void>> futures;
            for (int i = 0; i < 120; ++i)
            {
                auto promise = std::make_shared<std::promise<void>>();
                futures.push_back(promise->get_future());

                threadPool.add([this,i,promise]()
                               {
                                   PrecalculateBoneTransform(this->m_assimpRootNode,glm::mat4(1.0f),i);
                                   //testPrintf(to_string(i));

                                   promise->set_value();
                               });
            }

            // Wait for all tasks to complete
            for (auto& future : futures) {
                future.wait();
            }
        }

        void play()
        {
            if (animationClip)
            {
                if (state == SPW::AnimationState::Stopped)
                {
                    setState(AnimationState::Start);
                    resetTime();
                    startFromStop();
                }
            }
        }
        void startFromStop()
        {
            if (animationClip)
            {
                if (state == SPW::AnimationState::Start)
                {
                    setState(AnimationState::OnPlaying);
                    bUpdate = true;
                }
            }
        }
        void resume()
        {
            if (animationClip)
            {
                if (state == SPW::AnimationState::Paused)
                {
                    setState(SPW::AnimationState::OnPlaying);
                    bUpdate = true;
                }
            }
        }
        void pause()
        {
            if (animationClip)
            {
                if (state == SPW::AnimationState::OnPlaying)
                {
                    setState(SPW::AnimationState::Paused);
                    bUpdate = false;
                }
            }
        }
        void stop()
        {
            if (animationClip)
            {
                if (state != SPW::AnimationState::Stopped)
                {
                    setState(SPW::AnimationState::Stopped);
                    bUpdate = false;
                }
            }
        }
        void setState(AnimationState state)
        {
            this->state = state;
        }
        void resetTime()
        {
            currentTime = 0.0f;
        }
        void update(float dt)
        {
            if (bUpdate)
            {
                currentTime += tickPerSecond * dt * 0.5f;
                currentTime = fmod(currentTime,duration);

                int preIndex;
                int index;
                for (int i = 0; i < preload_finalBoneMatrices.size() - 1; ++i)
                {
                    if (preload_finalBoneMatrices[i+1].timeStamp > currentTime)
                    {
                        index = i+1 ;
                        preIndex = i;
                        break;
                    }
                }

                float preTime = preload_finalBoneMatrices[preIndex].timeStamp;
                float nextTime = preload_finalBoneMatrices[index].timeStamp;
                float midTime = currentTime;

                float left = midTime - preTime;
                float frameDua = nextTime - preTime;
                float factor = left / frameDua;

                for (int i = 0; i < finalBoneMatrices.size(); ++i)
                {
                    glm::mat4 preTransform = preload_finalBoneMatrices[preIndex].transformMatrix[i];
                    glm::mat4 lastTransform = preload_finalBoneMatrices[index].transformMatrix[i];

                    glm::vec3 translation, scaling;
                    glm::quat rotation;
                    glm::vec3 skew;
                    glm::vec4 perspective;
                    glm::decompose(preTransform, scaling, rotation, translation, skew, perspective);

                    glm::vec3 translation1, scaling1;
                    glm::quat rotation1;
                    glm::vec3 skew1;
                    glm::vec4 perspective1;
                    glm::decompose(lastTransform, scaling1, rotation1, translation1, skew1, perspective1);

                    glm::vec3 finalPosition = glm::mix(translation, translation1
                            , factor);

                    glm::quat finalRotation = glm::slerp(rotation, rotation1
                            , factor);

                    glm::vec3 finalScale = glm::mix(scaling, scaling1
                            , factor);

                    finalBoneMatrices[i] = glm::translate(glm::mat4(1.0f),finalPosition) * glm::toMat4(finalRotation) * glm::scale(glm::mat4(1.0f), finalScale);
                }
            }
        }

        bool isLoop;
        bool bUpdate;
        bool isPreLoaded;
        float currentTime;
        AnimationState state;
        std::vector<glm::mat4> finalBoneMatrices;
        std::vector<MatrixPerFrame> preload_finalBoneMatrices;
    public:
        const std::string &getAnimationName() const
        {
            return animationName;
        }

    private:
        void NameMappingBones(std::vector<std::shared_ptr<BoneInfo>> m_Bones)
        {
            int count = 0;
            for(std::weak_ptr<BoneInfo> boneInfo : m_Bones)
            {
                std::string name = boneInfo.lock()->name;

                if (m_BoneMap.find(name) != m_BoneMap.end())
                {
                    count++;
                }else
                {
                    m_BoneMap[name] = boneInfo.lock();
                }
            }
            printf("Number of repeated bones: %d",count);
        }
        void calculateBoneTransform(const AssimpNodeData& node,glm::mat4 parrentTransform)
        {
            std::string assimpName = node.name; //get name from assimp Node
            glm::mat4 localTransform = node.transformation; // get localTransform from assimpNode

            AnimationNode* currentNode = findAnimationNode(assimpName);
            if (currentNode)
            {
                localTransform = getUpdatedTransform(currentNode,currentTime);
            }
            localTransform = parrentTransform * localTransform;

            auto temp = findBoneInSkeleton(assimpName);
            if (temp)
            {
                uint32_t boneId = temp->boneID;
                auto boneOffset = temp->offsetMatrix;

                std::unique_lock<std::mutex>lock(mtx);
                finalBoneMatrices[boneId] = localTransform * boneOffset;
                lock.unlock();
            }

            for (int i = 0; i < node.childrenCount; ++i)
                calculateBoneTransform(node.children[i],localTransform);
        }

        void PrecalculateBoneTransform(const AssimpNodeData& node,glm::mat4 parrentTransform,int frame)
        {
            std::string assimpName = node.name; //get name from assimp Node
            glm::mat4 localTransform = node.transformation; // get localTransform from assimpNode

            AnimationNode* currentNode = findAnimationNode(assimpName);
            if (currentNode)
            {
                float time = preload_finalBoneMatrices[frame].timeStamp;
                localTransform = getUpdatedTransform(currentNode,time);
            }
            localTransform = parrentTransform * localTransform;

            auto temp = findBoneInSkeleton(assimpName);
            if (temp)
            {
                uint32_t boneId = temp->boneID;
                auto boneOffset = temp->offsetMatrix;

                std::unique_lock<std::mutex>lock(mtx);
                preload_finalBoneMatrices[frame].transformMatrix[boneId] = localTransform * boneOffset;
                lock.unlock();
            }

            for (int i = 0; i < node.childrenCount; ++i)
                PrecalculateBoneTransform(node.children[i],localTransform,frame);
        }


        glm::mat4 getUpdatedTransform(AnimationNode* node,float currentTime)
        {

            glm::mat4 transform;
            //Get transform here
            glm::mat4 position = node->InterpolatePosition(currentTime);
            glm::mat4 rotation = node->InterpolateRotation(currentTime);
            glm::mat4 scaling = node->InterpolateScaling(currentTime);

            transform = position * rotation * scaling;
            return transform;
        }
        SPW::AnimationNode* findAnimationNode(std::string name)
        {
            auto iter = std::find_if
                    (animationClip->nodeAnimations.begin(),
                     animationClip->nodeAnimations.end(),[&]
                             (const SPW::AnimationNode node)
                     {
                         return node.nodeName == name;
                     });
            if (iter == animationClip->nodeAnimations.end())
                return nullptr;
            else
                return &(*iter);
        }
        std::shared_ptr<SPW::BoneInfo> findBoneInSkeleton(std::string animNodeName)
        {

            auto iter = m_BoneMap.find(animNodeName);
            if (iter == m_BoneMap.end())
                return nullptr;
            else
                return iter->second;
        }

        void testPrintf(std::string test)
        {
            printf(test.c_str());
        }

    private:
        std::map<std::string,std::shared_ptr<BoneInfo>> m_BoneMap;
        std::shared_ptr<AnimationClip> animationClip;
        AssimpNodeData m_assimpRootNode;
        std::string animationName;
        float duration;
        int tickPerSecond;

        std::mutex mtx;
    };



    //SPW::SPWVertexBoneMap class
    //顶点映射类
    class SPWVertexBoneMap
    {
    public:
        SPWVertexBoneMap() = default;
        explicit SPWVertexBoneMap(std::vector<std::shared_ptr<BoneInfo>> m_Bones, std::weak_ptr<Model> model)
        {
            m_NumBones = m_Bones.size();
            ExtractWeightFromBones(model,m_Bones);
        }

        void ExtractWeightFromBones(std::weak_ptr<Model> model,std::vector<std::shared_ptr<BoneInfo>> m_BoneMap)
        {
            unsigned int numVertex = 0;
            std::vector<VerMapBone> verMapBone;
            for(std::weak_ptr<Mesh> mesh : model.lock()->GetMeshes())
            {
                numVertex+=mesh.lock()->vertices.size();
            }

            verMapBone.resize(numVertex);
            for(auto boneInfo : m_BoneMap)
            {
                for(Weight weight : boneInfo->weights)
                {
                    uint32_t vertexID = weight.vertexID;
                    uint32_t boneID = boneInfo->boneID;
                    float value = weight.value;

                    verMapBone[vertexID].boneID.push_back(boneID);
                    verMapBone[vertexID].weight.push_back(value);
                }
            }

            //Get number of vertices
            startIndex.reserve(verMapBone.size());

            //For every vertex
            int startInd = 0;
            for (int i = 0; i < verMapBone.size(); ++i)
            {
                auto temp = verMapBone[i];
                startIndex.push_back(startInd);

                for (int j = 0; j < temp.boneID.size(); j++)
                {
                    boneID.push_back(temp.boneID[j]);
                    weights.push_back(temp.weight[j]);
                }
                startInd += temp.boneID.size();
                size.push_back(temp.boneID.size());
            }
        }

        //Buffer information
        std::vector<uint32_t> startIndex;
        std::vector<uint32_t> size;
        std::vector<float> weights;
        std::vector<uint32_t> boneID;
        int m_NumBones;
    };


    class SPWAnimSSBO{
    public:
        SPWAnimSSBO() = default;
        explicit  SPWAnimSSBO(std::shared_ptr<SPW::SPWVertexBoneMap> SPW_VertexMap)
        {
            initializeBuffer(SPW_VertexMap);
        }


        void initializeBuffer(std::shared_ptr<SPW::SPWVertexBoneMap> SPW_VertexMap)
        {
            starts = std::make_shared<StorageBuffer>(
                    "WeightDictStart",
                    SPW_VertexMap->startIndex.size() * sizeof(int),
                    0);

            sizes = std::make_shared<StorageBuffer>(
                    "WeightDictSize",
                    SPW_VertexMap->size.size() * sizeof(int),
                    1);

            boneIndices = std::make_shared<StorageBuffer>(
                    "WeightDictKey",
                    SPW_VertexMap->boneID.size() * sizeof(int),
                    2);

            weights = std::make_shared<StorageBuffer>(
                    "WeightDictValue",
                    SPW_VertexMap->weights.size() * sizeof(float),
                    3);

            mats = std::make_shared<StorageBuffer>(
                    "WeightMatries",
                    SPW_VertexMap->m_NumBones * sizeof(glm::mat4),
                    4);
            bInitialized = true;
        }

        void updateStaticBuffer(std::shared_ptr<SPW::SPWVertexBoneMap> SPW_VertexMap)
        {
            starts->updateSubData(SPW_VertexMap->startIndex.data(),
                                  0,
                                  SPW_VertexMap->startIndex.size()*sizeof(int));
            sizes->updateSubData(
                    SPW_VertexMap->size.data(),
                    0,
                    SPW_VertexMap->size.size() * sizeof(int));

            boneIndices->updateSubData(
                    SPW_VertexMap->boneID.data(),
                    0,
                    SPW_VertexMap->boneID.size() * sizeof(int));

            weights->updateSubData(
                    SPW_VertexMap->weights.data(),
                    0,
                    SPW_VertexMap->weights.size() * sizeof(float));
        }

        void updateDynamicBuffer(std::weak_ptr<SPW::SPWAnimation> onGoingAnim)
        {
            if (!onGoingAnim.expired())
            {
                if (bInitialized)
                {
                    mats->updateSubData(
                            onGoingAnim.lock()->finalBoneMatrices.data(),
                            0,
                            onGoingAnim.lock()->finalBoneMatrices.size() * sizeof(glm::mat4)
                    );
                }
            }
        }

        void bindingBuffer(MeshComponent* mesh)
        {
            mesh->beforeDraw = [this](RenderCommandsQueue<RenderBackEndI>& queue) {
                queue.pushCommand(
                        RenderCommand(&RenderBackEndI::initStorageBuffer,
                                      starts));
                queue.pushCommand(
                        RenderCommand(&RenderBackEndI::initStorageBuffer,
                                      sizes));
                queue.pushCommand(
                        RenderCommand(&RenderBackEndI::initStorageBuffer,
                                      boneIndices));
                queue.pushCommand(
                        RenderCommand(&RenderBackEndI::initStorageBuffer,
                                      weights));
                queue.pushCommand(
                        RenderCommand(&RenderBackEndI::initStorageBuffer,
                                      mats));
            };
            bBinding = true;
        }

        void pushData(MeshComponent* mesh)
        {
            mesh->onDraw = [this](RenderCommandsQueue<Shader>& queue) {
                queue.pushCommand(RenderCommand(&Shader::setStorageBuffer, starts));
                queue.pushCommand(RenderCommand(&Shader::setStorageBuffer, sizes));
                queue.pushCommand(RenderCommand(&Shader::setStorageBuffer, boneIndices));
                queue.pushCommand(RenderCommand(&Shader::setStorageBuffer, weights));
                queue.pushCommand(RenderCommand(&Shader::setStorageBuffer, mats));
            };
        }

        std::shared_ptr<StorageBuffer> starts;
        std::shared_ptr<StorageBuffer> sizes;
        std::shared_ptr<StorageBuffer> boneIndices;
        std::shared_ptr<StorageBuffer> weights;
        std::shared_ptr<StorageBuffer> mats;
        bool bBinding = false;
        bool bInitialized = false;
    };


    class AnimationComponent : ComponentI
    {
    public:
        //Constructor
        AnimationComponent() = default;

        explicit AnimationComponent(const std::shared_ptr<SPW::Skeleton> data)
        {
            for(int i = 0 ; i < data->m_animClips.size();i++)
            {
                auto animationClip = std::make_shared<SPWAnimation>(data->m_animClips[i],data);
                allAnimations.insert({animationClip->getAnimationName(),animationClip});
            }

            skeleton = data;
            isLoaded = true;
        };

        //Take action on current Anim
        void respondAction(SPW::AnimationAction action,std::string animationName = "")
        {
            switch (action)
            {
                case AnimationAction::Swap:
                    swapCurrentAnim(animationName);
                    break;
                case AnimationAction::Pause:
                    if (!onGoingAnim.expired())
                        onGoingAnim.lock()->pause();
                    break;
                case AnimationAction::Reset:
                    if (!onGoingAnim.expired())
                        onGoingAnim.lock()->resetTime();
                    break;
                case AnimationAction::Resume:
                    if (!onGoingAnim.expired())
                        onGoingAnim.lock()->resume();
                    break;
            }
        }

        void initializeMapping(std::weak_ptr<Model> model)
        {
            if (skeleton)
                SPW_VertexMap = std::make_shared<SPW::SPWVertexBoneMap>(skeleton->m_Bones,model);

            if (SPW_VertexMap)
                SPW_AnimSSBO = std::make_shared<SPW::SPWAnimSSBO>(SPW_VertexMap);

            mapInitialize = true;
        }


        bool isLoaded = false;
        bool mapInitialize = false;

        //swap animation
        void swapCurrentAnim(const std::string &name)
        {
            auto result = allAnimations.find(name);
            if (result != allAnimations.end())
            {
                //stop current Anim
                if (!onGoingAnim.expired())
                {
                    onGoingAnim.lock()->stop();
                    onGoingAnim.reset();
                }

                //Release reference and assign a new one
                if (onGoingAnim.expired())
                {
                    onGoingAnim = result->second;
                }

                onGoingAnim.lock()->play();
            }
        }

        void addAnimation(std::shared_ptr<AnimationClip> clip)
        {
            if (!skeleton && clip)
            {
                std::shared_ptr<SPW::SPWAnimation> newClip = std::make_shared<SPW::SPWAnimation>(clip,skeleton);
                allAnimations.insert({newClip->getAnimationName(),newClip});
            }
        }
        void setLoop(bool enable)
        {
            if (!onGoingAnim.expired())
                onGoingAnim.lock()->isLoop = enable;
        }

        std::unordered_map<std::string, std::shared_ptr<SPW::SPWAnimation>> allAnimations;
        std::shared_ptr<SPW::SPWAnimSSBO> SPW_AnimSSBO;
        std::shared_ptr<SPW::SPWVertexBoneMap> SPW_VertexMap;
        std::weak_ptr<SPW::SPWAnimation> onGoingAnim;

        //Original data
        std::shared_ptr<Skeleton> skeleton;
    };
}

#endif //SPARROW_ANIMATIONCOMPONENT_H



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

namespace SPW
{
	//Start: Start an animation from 'Stopped' state or 'Paused' state
	//Reset the current time and change to OnPlaying
	//Stopped: Animation is stopped at time: 0
	//OnPlaying -> stop: bUpdate = false; (AnimComponent to                           animation)
	//Paused: Animation is stopped during playing
	//Onplaying: Animation is playing and keeps updating
	class SPWAnimationDelegateI
	{
	public:
		virtual std::weak_ptr<AnimationClip> playAnim(const std::string& name, bool isLoop);
	};


	//SPW::SPWVertexBoneMap class
	//顶点映射类
	class SPWVertexBoneMap
	{
	public:
		SPWVertexBoneMap() = default;

		explicit SPWVertexBoneMap(std::vector<BoneInfo> m_Bones, std::weak_ptr<Model> model)
		{
			m_NumBones = m_Bones.size();
			ExtractWeightFromBones(model, m_Bones);
		}

		void ExtractWeightFromBones(std::weak_ptr<Model> model, std::vector<BoneInfo> m_BoneMap)
		{
			unsigned int numVertex = 0;
			std::vector<VerMapBone> verMapBone;
			for (std::weak_ptr<Mesh> mesh : model.lock()->GetMeshes())
			{
				numVertex += mesh.lock()->vertices.size();
			}

			verMapBone.resize(numVertex);
			for (auto boneInfo : m_BoneMap)
			{
				for (Weight weight : boneInfo.weights)
				{
					uint32_t vertexID = weight.vertexID;
					uint32_t boneID = boneInfo.boneID;
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

	class SPWAnimSSBO
	{
	public:
		SPWAnimSSBO() = default;

		explicit SPWAnimSSBO(std::shared_ptr<SPW::SPWVertexBoneMap> SPW_VertexMap)
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
			                      SPW_VertexMap->startIndex.size() * sizeof(int));
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

		void updateDynamicBuffer(AnimationClip* onGoingAnim)
		{
			if (onGoingAnim)
			{
				if (bInitialized)
				{
					mats->updateSubData(
						onGoingAnim->finalBoneMatrices.data(),
						0,
						onGoingAnim->finalBoneMatrices.size() * sizeof(glm::mat4)
					);
				}
			}
		}

		void bindingBuffer(MeshComponent* mesh)
		{
			mesh->beforeDraw = [this](RenderCommandsQueue<RenderBackEndI>& queue)
			{
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
			mesh->onDraw = [this](RenderCommandsQueue<Shader>& queue)
			{
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

		explicit AnimationComponent(const std::shared_ptr<Skeleton>& data)
		{
<<<<<<< HEAD
			for (auto& animClip : data->animClips)
			{
				allAnimations.insert({animClip.name , animClip});
=======
			for (int i = 0; i < data->animClips.size(); i++)
			{
				allAnimations.insert({ data->animClips[i].name , data->animClips[i] });
>>>>>>> ff1e267... OPT: REVISE CODE STRUCTURE A LOT FOR SER
			}

			skeleton = data;
			isLoaded = true;
<<<<<<< HEAD
		}

		~AnimationComponent() { onGoingAnim = nullptr; }
=======
		};
>>>>>>> ff1e267... OPT: REVISE CODE STRUCTURE A LOT FOR SER

		//Take action on current Anim
		void respondAction(SPW::AnimationAction action, std::string animationName)
		{
			switch (action)
			{
			case AnimationAction::Swap:
				swapCurrentAnim(animationName);
				break;
			case AnimationAction::Pause:
				if (onGoingAnim)
					onGoingAnim->pause();
				break;
			case AnimationAction::Reset:
				if (onGoingAnim)
					onGoingAnim->resetTime();
				break;
			case AnimationAction::Resume:
				if (onGoingAnim)
					onGoingAnim->resume();
				break;
			}
		}

		void initializeMapping(std::weak_ptr<Model> model)
		{
			if (skeleton)
				SPW_VertexMap = std::make_shared<SPWVertexBoneMap>(skeleton->m_Bones, model);

			if (SPW_VertexMap)
				SPW_AnimSSBO = std::make_shared<SPWAnimSSBO>(SPW_VertexMap);

			mapInitialize = true;
		}


		bool isLoaded = false;
		bool mapInitialize = false;

		//swap animation
		void swapCurrentAnim(const std::string& name)
		{
			const auto result = allAnimations.find(name);
			if (result != allAnimations.end())
			{
				//stop current Anim
				if (onGoingAnim)
				{
					onGoingAnim->stop();
					onGoingAnim = nullptr;
				}

				//Release reference and assign a new one
				if (!onGoingAnim)
				{
					onGoingAnim = &result->second;
				}

				onGoingAnim->play();
			}
		}

		void addAnimation(AnimationClip clip)
		{
			if (!skeleton && !clip.name.empty())
			{
				allAnimations.insert({clip.name, clip});
			}
		}

		void setLoop(bool enable)
		{
			if (onGoingAnim)
				onGoingAnim->isLoop = enable;
		}

		void SetAnimation(AnimationClip* anim)
		{
			onGoingAnim = anim;
		}


		std::unordered_map<std::string, AnimationClip> allAnimations;
		std::shared_ptr<SPWAnimSSBO> SPW_AnimSSBO;
		std::shared_ptr<SPWVertexBoneMap> SPW_VertexMap;
		AnimationClip* onGoingAnim = nullptr;

		//Original data
		std::shared_ptr<Skeleton> skeleton;
	};
}

#endif //SPARROW_ANIMATIONCOMPONENT_H

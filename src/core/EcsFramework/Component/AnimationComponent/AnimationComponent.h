//
// Created by Shawwy on 2/24/2023.
//
#ifndef SPARROW_ANIMATIONCOMPONENT_H
#define SPARROW_ANIMATIONCOMPONENT_H
#include "../ComponentI.h"
#include "EcsFramework/Component/MeshComponent.hpp"
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include "glm/glm.hpp"
#include "Render/StorageBuffer.h"
#include "Utils/ThreadPool.h"
#include <future>
#include <glm/gtx/matrix_decompose.hpp>
#include "Asset/ResourceManager/ResourceManager.h"

namespace SPW
{
	//Start: Start an animation from 'Stopped' state or 'Paused' state
	//Reset the current time and change to OnPlaying
	//Stopped: Animation is stopped at time: 0
	//OnPlaying -> stop: bUpdate = false; (AnimComponent to animation)
	//Paused: Animation is stopped during playing
	//Onplaying: Animation is playing and keeps updating
	class SPWAnimationDelegateI
	{
	public:
		virtual std::weak_ptr<AnimationClip> playAnim(const std::string& name, bool isLoop);
	};


	class SPWAnimSSBO
	{
	public:
		SPWAnimSSBO() = default;

		explicit SPWAnimSSBO(const VertBoneMap& SPW_VertexMap)
		{
			starts = std::make_shared<StorageBuffer>(
				"WeightDictStart",
				SPW_VertexMap.startIndex.size() * sizeof(int),
				0);

			sizes = std::make_shared<StorageBuffer>(
				"WeightDictSize",
				SPW_VertexMap.count.size() * sizeof(int),
				1);

			boneIndices = std::make_shared<StorageBuffer>(
				"WeightDictKey",
				SPW_VertexMap.boneID.size() * sizeof(int),
				2);

			weights = std::make_shared<StorageBuffer>(
				"WeightDictValue",
				SPW_VertexMap.weights.size() * sizeof(float),
				3);

			mats = std::make_shared<StorageBuffer>(
				"WeightMatries",
				SPW_VertexMap.boneCount * sizeof(glm::mat4),
				4);
			bInitialized = true;
		}

		void updateStaticBuffer(const VertBoneMap& SPW_VertexMap)
		{
			starts->updateSubData((void*)SPW_VertexMap.startIndex.data(),
			                      0,
			                      SPW_VertexMap.startIndex.size() * sizeof(int));
			sizes->updateSubData(
				(void*)SPW_VertexMap.count.data(),
				0,
				SPW_VertexMap.count.size() * sizeof(int));

			boneIndices->updateSubData(
				(void*)SPW_VertexMap.boneID.data(),
				0,
				SPW_VertexMap.boneID.size() * sizeof(int));

			weights->updateSubData(
				(void*)SPW_VertexMap.weights.data(),
				0,
				SPW_VertexMap.weights.size() * sizeof(float));
		}

		void updateDynamicBuffer(AnimationClip* onGoingAnim)
		{
			if (onGoingAnim)
			{
				if (bInitialized)
				{
					mats->updateSubData(
						onGoingAnim->boneMatrices.data(),
						0,
						onGoingAnim->boneMatrices.size() * sizeof(glm::mat4)
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

	class AnimationComponent : public ComponentI
	{
	public:
		//Constructor
		AnimationComponent() = default;
        AnimationComponent(const AnimationComponent&) = default;

		explicit AnimationComponent(const std::string& asset_name)
		{
			assetName = asset_name;
			const auto skeleton = ResourceManager::getInstance()->m_AssetDataMap[assetName].skeleton;

			for (auto& animClip : skeleton.animClips)
			{
				allAnimations.insert({animClip.name , animClip});
			}

			SPW_AnimSSBO = std::make_shared<SPWAnimSSBO>(skeleton.vertexWeightMap);
		}

		~AnimationComponent() { onGoingAnim = nullptr; }

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


        void setState(AnimationState state)
        {
            if (onGoingAnim)
            {
                onGoingAnim->setState(state);
            }
        }


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
			// if (!skeleton && !clip.name.empty())
			// {
			// 	allAnimations.insert({clip.name, clip});
			// }
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


        void initFromLua(const sol::table& value) override
        {
            if (!value["animFilePath"].valid())
                return;
            std::string assetPath = value["animFilePath"];

            assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap[assetPath].assetName;

            const auto skeleton = ResourceManager::getInstance()->m_AssetDataMap[assetName].skeleton;

            for (auto& animClip : skeleton.animClips)
            {
                allAnimations.insert({animClip.name , animClip});
            }
            SPW_AnimSSBO = std::make_shared<SPWAnimSSBO>(skeleton.vertexWeightMap);
        }


        void update(const std::string& key, const sol::table& value) override
        {

            if (key == "enableLoop" && value["value"].valid())
            {
                bool isLoop = value["value"];
                setLoop(isLoop);
            }
            else if (key == "disableLoop" && value["value"].valid())
            {
                bool isLoop = value["value"];
                setLoop(isLoop);
            }
            else if(key == "swapAnimation" && value["value"].valid())
            {
                string name  = value["value"];
                swapCurrentAnim(name);

            }else if (key == "setState" && value["value"].valid())
            {
                int state =  value["value"];
                setState((AnimationState)state);
            }
        }

        virtual sol::object getLuaValue(const sol::table& value, const std::string& key) override
        {
            if (key == "animFiles")
            {
                sol::table animFiles = sol::state_view(value.lua_state()).create_table();
                animFiles[onGoingAnim->name] = onGoingAnim->state;
                return animFiles;
            }
            return sol::nil;
        }

        std::unordered_map<std::string, AnimationClip> allAnimations;
		std::shared_ptr<SPWAnimSSBO> SPW_AnimSSBO;
		AnimationClip* onGoingAnim = nullptr;

		//Original data
		// Skeleton* skeleton;

		std::string assetName;
	};







}

#endif //SPARROW_ANIMATIONCOMPONENT_H

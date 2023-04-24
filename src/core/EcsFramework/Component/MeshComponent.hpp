#pragma once

#include "ComponentI.h"
#include "Render/PipeLine.hpp"
#include <string>
#include <unordered_set>
#include "Render/RenderGraph.hpp"
#include "Render/RenderCommandsQueue.hpp"
#include "Utils/UUID.hpp"
#include "Asset/Asset.hpp"
#include <cereal/types/memory.hpp>
#include "Asset/ResourceManager/ResourceManager.h"

#include "Asset/AssetData/AssetData.h"
#include "Asset/AssetData/MaterialData.h"

namespace SPW
{
	// MESH RENDERER
	class MeshComponent : public ComponentI
	{
	public:
		MeshComponent() = default;

		explicit MeshComponent(const UUID& bind_camera)
		{
			bindCamera = bind_camera;
		}

		void update(const std::string& key, const sol::table& value) final
		{
			if (!value["value"].valid())
				return;
			if (key == "mesh_path")
			{
				std::string path = value["value"];
				// model = ResourceManager::getInstance()->LoadModel(path);
				// ready = false;
			}
			else if (key == "cam_id")
			{
				std::string id = value["value"];
				bindCamera = UUID::fromString(id.c_str());
			}
			else if (key == "graph_id")
			{
				unsigned int id = value["value"];
				bindRenderGraph = id;
			}
			else if (key == "renderPrograms")
			{
				unsigned int id = value["value"]["pass_id"];
				std::string program = value["value"]["shader_id"];
				modelSubPassPrograms[id] = UUID::fromString(program.c_str());
			}
		}


		void initFromLua(const sol::table& value)
		{
			if (value["mesh_path"].valid())
			{
				std::string path = value["mesh_path"];
				// model = ResourceManager::getInstance()->LoadModel(path);
				ready = false;
			}
			if (value["cam_id"].valid())
			{
				std::string id = value["cam_id"];
				bindCamera = UUID::fromString(id.c_str());
			}
			if (value["graph_id"].valid())
			{
				unsigned int id = value["graph_id"];
				bindRenderGraph = id;
			}
		}

		void BindCamera(const UUID& camera_id)
		{
			bindCamera = camera_id;
		}


		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("bindCamera", bindCamera.toString()),
				cereal::make_nvp("bindRenderPass", bindRenderPass),
				cereal::make_nvp("bindRenderGraph", bindRenderGraph),
				cereal::make_nvp("ready", ready),
				cereal::make_nvp("assetID", assetID)
			);
		}

		// getLuaValue
		virtual sol::object getLuaValue(const sol::table& value, const std::string& key)
		{
			return sol::nil;
		}

		// render callback
		std::function<void(RenderCommandsQueue<Shader>& queue)> onDraw;
		std::function<void(RenderCommandsQueue<RenderBackEndI>& queue)> beforeDraw;

		UUID bindCamera;
		// render pass id -> subpass ref -> shader
		unsigned int bindRenderPass = 0;
		std::unordered_map<unsigned int, ShaderHandle> modelSubPassProgram;

		unsigned int bindRenderGraph = 0;
		std::unordered_map<unsigned int, UUID> modelSubPassPrograms;

		bool ready = false;

		// -------------- NEW ASSET DATA --------------
		std::string assetPath{};
		std::string assetID{};
		std::string assetName;
	};
}

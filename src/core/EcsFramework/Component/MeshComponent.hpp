#pragma once

#include "ComponentI.h"
#include "Model/Model.h"
#include "Render/PipeLine.hpp"
#include <string>
#include <unordered_set>
#include "Render/RenderGraph.hpp"
#include "Utils/UUID.hpp"
#include "Asset/Asset.hpp"
#include <cereal/types/memory.hpp>

#include "Asset/AssetData/MaterialData.h"

namespace SPW {
    // MESH RENDERER
    class MeshComponent : public ComponentI {
    public:
        MeshComponent() = default;

        explicit MeshComponent(const UUID &id) {
            bindCamera = id;
        }

        void update(const std::string &key, const sol::table &value) final {

        }


    	template<class Archive>
        void serialize(Archive& ar)
        {
            ar(
                cereal::make_nvp("bindCamera", bindCamera.toString()),
                cereal::make_nvp("bindRenderPass", bindRenderPass),
                cereal::make_nvp("bindRenderGraph", bindRenderGraph),
                cereal::make_nvp("ready", ready),
                cereal::make_nvp("model", model),
                cereal::make_nvp("assetID", assetID)
            );
        }

    	void initFromLua(const sol::table &value) {

        }

        // getLuaValue
        virtual sol::object getLuaValue(const sol::table &value, const std::string &key) {
            return sol::nil;
        }

        UUID bindCamera;
        // render pass id -> subpass ref -> shader
        unsigned int bindRenderPass = 0;
        std::unordered_map<unsigned int, ShaderHandle> modelSubPassProgram;

        unsigned int bindRenderGraph = 0;
        std::unordered_map<unsigned int, UUID> modelSubPassPrograms;

        bool ready = false;
        std::shared_ptr<Model> model;

        // ---------------------- NEW DATA ---------------------------------

		/*
		 * 1. Load Asset, Raise Data from .asset file, to complish meshes/materials, pair meshes and materials with UUID
		 * 2. Save Asset, Rewrite the .asset file, save changes(just new file into .bin and .asset)
		 */
        bool 					    b_Asset = false;
        std::string                 assetPath{};
        std::string 			    assetID{};
    	std::string                 meshURI; // just use this component UUID

    	std::vector<Mesh>           meshes;
        std::vector<MaterialData>   materials;  /* which is the brand new material! */
        std::unordered_map<std::string, std::string> textures;

    };
}

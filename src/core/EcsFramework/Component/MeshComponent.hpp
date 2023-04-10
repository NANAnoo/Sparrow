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

namespace SPW {
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
            ar(cereal::make_nvp("bindCamera", bindCamera.toString()),
                cereal::make_nvp("bindRenderPass", bindRenderPass),
                cereal::make_nvp("bindRenderGraph", bindRenderGraph),
                cereal::make_nvp("ready", ready),
                cereal::make_nvp("model_ptr", model)
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
    };
}

#pragma once

#include "ComponentI.h"
#include "Model/Model.h"
#include "Render/PipeLine.hpp"
#include <string>
#include <unordered_set>
#include "Render/RenderGraph.hpp"
#include "Utils/UUID.hpp"

namespace SPW {
    class MeshComponent : public ComponentI {
    public:
        MeshComponent() = delete;

        explicit MeshComponent(const UUID &id) {
            bindCamera = id;
        }

        void update(const std::string &key, const sol::table &value) final {

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

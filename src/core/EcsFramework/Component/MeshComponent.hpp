#pragma once

#include "ComponentI.h"
#include "Model/Model.h"
#include "Render/PipeLine.hpp"
#include <string>
#include <unordered_set>
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
        
        // render values
        std::unordered_map<unsigned int, std::unordered_map<std::string, float>> float_constants;
        std::unordered_map<unsigned int, std::unordered_map<std::string, glm::vec4>> vec4_constants;
        std::unordered_map<unsigned int, std::unordered_map<std::string, glm::mat4>>mat4_constants;

        UUID bindCamera;
        // render pass id -> subpass ref -> shader
        unsigned int bindRenderPass = 0;
        std::unordered_map<unsigned int, ShaderHandle> modelSubPassProgram;

        bool ready = false;
        std::shared_ptr<Model> model;
    };
}

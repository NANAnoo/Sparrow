//
// Created by ZhangHao on 2023/2/20.
//

#pragma once

#include "ComponentI.h"
#include "Model/Model.h"
#include "Render/RenderCommandsQueue.hpp"
#include <string>
#include <unordered_set>
#include "Utils/UUID.hpp"

namespace SPW {
    class ModelComponent : public ComponentI {
    public:
        ModelComponent() = delete;
        explicit ModelComponent(const UUID &id) {
            bindCameras.insert(id);
        }
        ShaderHandle shadowProgram;
        ShaderHandle modelProgram;
        std::shared_ptr<Model> model;
        bool ready = false;
        std::unordered_set<UUID, UUID::hash> bindCameras;

        RenderCommandsQueue<RenderBackEndI> preRenderCommands;
        RenderCommandsQueue<Shader> pipeLineCommands;
    };
}

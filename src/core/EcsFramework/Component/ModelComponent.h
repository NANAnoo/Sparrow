//
// Created by ZhangHao on 2023/2/20.
//

#pragma once

#include "ComponentI.h"
#include "Model/Model.h"
#include <string>

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
    };
}

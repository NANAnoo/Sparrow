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
        ModelComponent() = default;
        std::string name;
        std::string vertex_shader_path;
        std::string frag_shader_path;
        std::shared_ptr<Model> model;
        bool ready = false;
    };
}

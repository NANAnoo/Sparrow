//
// Created by ZhangHao on 2023/2/20.
//

#pragma once

#include "ComponentI.h"
#include "Model/Model.h"
#include <string>
#include <unordered_set>
#include "Utils/UUID.hpp"
#include "IO/ResourceManager.h"

namespace SPW {
    class ModelComponent : public ComponentI {
    public:
        ModelComponent() = delete;
        explicit ModelComponent(const UUID &id) {
            bindCameras.insert(id);
        }

        void update(const std::string &key, const sol::table &value) final {
            if (key == "path" && value["value"].valid()) {
                std::string path = value["value"];
                model = ResourceManager::getInstance()->LoadModel(path);
                ready = false;
            }
            if (key == "shadow_handle" && value["value"].valid()) {
                shadowProgram = value["value"];
            }  
            if (key == "object_handle" && value["value"].valid()) {
                modelProgram = value["value"];
            }
        }

        void initFromLua(const sol::table &value) {
            if (value["path"].valid()) {
                std::string path = value["path"];
                model = ResourceManager::getInstance()->LoadModel(path);
                ready = false;
            }
             if (value["camera_id"].valid()) {
                std::string id = value["camera_id"];
                bindCameras.insert(UUID::fromString(id.c_str()));
            }
        }

        // getLuaValue
        virtual sol::object getLuaValue(const sol::table &value, const std::string &key) {
            if (key == "path") {
                return sol::make_object(value.lua_state(), model->getFilePath());
            }
            if (key == "shadow_handle") {
                return sol::make_object(value.lua_state(), shadowProgram);
            }
            if (key == "object_handle") {
                return sol::make_object(value.lua_state(), modelProgram);
            }
            return sol::nil;
        }

        ShaderHandle shadowProgram;
        ShaderHandle modelProgram;
        std::shared_ptr<Model> model;
        bool ready = false;
        std::unordered_set<UUID, UUID::hash> bindCameras;
    };
}

//
// Created by duhao on 2023/3/27.
//

#ifndef SPARROW_RIGIDACTOR_H
#define SPARROW_RIGIDACTOR_H

#endif //SPARROW_RIGIDACTOR_H
#include "../ComponentI.h"
#include "EcsFramework/System/PhysicSystem/PhysicSystem.h"
#include <PxPhysicsAPI.h>
#include <memory>
#include <iostream>

namespace SPW{

    enum RigidState{Sleep,Awake,Needupdate};


    class RigidStaticComponent : public ComponentI{
    public:
        physx::PxRigidActor* rigid;
        RigidState rigidState = Sleep;
        // init from lua
        void initFromLua(const sol::table &value) override {
            if (value["rigidState"].valid())
                rigidState = value["rigidState"];
        }
        //
        // getLuaValue
        void update(const std::string &key, const sol::table &value) final {
            if (key == "rigidState" && value["value"].valid()) {
                rigidState = value["value"];
            }


        }



        virtual sol::object getLuaValue(const sol::table &value, const std::string &key) override {
            if (key == "rigidState") {
                return sol::make_object(value.lua_state(), rigidState);
            }
            return sol::nil;
        }


    };

    class RigidDynamicComponent : public ComponentI{
    public:
        physx::PxRigidBody* rigid;
        RigidState rigidState = Sleep;
        // init from lua
        void initFromLua(const sol::table &value) override {
            if (value["rigidState"].valid())
                rigidState = value["rigidState"];
        }
        //
        // update from lua
        void update(const std::string &key, const sol::table &value) final {
            if (key == "rigidState" && value["value"].valid()) {
                rigidState = value["value"];

            }

        }

        virtual sol::object getLuaValue(const sol::table &value, const std::string &key) override {
            if (key == "rigidState") {
                return sol::make_object(value.lua_state(), rigidState);
            }
            return sol::nil;
        }
    private:
      bool enable_ccd_;
    };
};
//
// Created by duhao on 2023/3/27.
//

#ifndef SPARROW_COLLIDER_H
#define SPARROW_COLLIDER_H

#include <PxPhysicsAPI.h>
#include <memory>
#include "../ComponentI.h"
#include "Physics/PhysicMaterial.h"
#include "EcsFramework/System/PhysicSystem/PhysicSystem.h"

namespace SPW{

    enum ColliderState{needSleep,needAwake,sleep,awake};

    class Collider : public ComponentI{

        friend class PhysicSystem;

    public:
        Collider()=default;
        ~Collider()=default;
        ColliderState state = sleep;
        std::shared_ptr<PhysicMaterial> custom_material_;
        bool is_trigger_ = false;


        std::function<void(const SPW::Entity* e)> trigger_enter = nullptr;
        std::function<void(const SPW::Entity* e)> trigger_leave;
        std::function<void(const SPW::Entity* e1,const SPW::Entity* e2)> contact_enter;
        std::function<void(const SPW::Entity* e1,const SPW::Entity* e2)> contact_leave;

        // init from lua
        void initFromLua(const sol::table &value) override {
            if (value["trigger_enter"].valid()) {
                trigger_enter = [value](const SPW::Entity* e) {
                    value["trigger_enter"](e->getUUID().toString());
                };
            }
            if (value["trigger_leave"].valid()) {
                trigger_leave = [value](const SPW::Entity* e) {
                    value["trigger_leave"](e->getUUID().toString());
                };
            }
            if (value["contact_enter"].valid()) {
                contact_enter = [value](const SPW::Entity* e1, const SPW::Entity* e2) {
                    value["contact_enter"](e1->getUUID().toString(), e2->getUUID().toString());
                };
            }
            if (value["contact_leave"].valid()) {
                contact_leave = [value](const SPW::Entity* e1, const SPW::Entity* e2) {
                    value["contact_leave"](e1->getUUID().toString(), e2->getUUID().toString());
                };
            }

        }


        // update from lua
        void update(const std::string &key, const sol::table &value) final {

            if (key == "state" && value["value"].valid()) {
                state = value["value"];
            }

            if (key == "trigger_enter") {
                trigger_enter = [value](const SPW::Entity* e) {
                    value["value"](e->getUUID().toString());
                };
            }
            if (key == "trigger_leave") {
                trigger_leave = [value](const SPW::Entity* e) {
                    value["value"](e->getUUID().toString());
                };
            }
            if (key == "contact_enter") {
                contact_enter = [value](const SPW::Entity* e1, const SPW::Entity* e2) {
                    value["value"](e1->getUUID().toString(), e2->getUUID().toString());
                };
            }
            if (key == "contact_leave") {
                contact_leave = [value](const SPW::Entity* e1, const SPW::Entity* e2) {
                    value["value"](e1->getUUID().toString(), e2->getUUID().toString());
                };
            }

        }


    protected:

        physx::PxMaterial* px_material_;



    };

    class SphereCollider : public Collider{
        friend class PhysicSystem;
    public:
        float sphere_radius_;

        // init from lua
        void initFromLua(const sol::table &value) override {
            if (value["sphere_radius_"].valid())
                sphere_radius_ = value["sphere_radius_"];
            if (value["state_"].valid())
                state = value["state_"];
            if (value["is_trigger_"].valid())
                is_trigger_ =value["is_trigger_"];

        }





        virtual sol::object getLuaValue(const sol::table &value, const std::string &key) override {
            if (key == "sphere_radius_") {
                return sol::make_object(value.lua_state(), sphere_radius_);
            }
            if (key == "state_") {
                return sol::make_object(value.lua_state(), state);
            }
            if (key == "is_trigger_") {
                return sol::make_object(value.lua_state(), is_trigger_);
            }
            return sol::nil;
        }
    };


    class CapsuleCollider : public Collider{
        friend class PhysicSystem;
    public:

        float capsule_radius_;
        float capsule_half_height_;
        // Adjust capsule shape
        glm::vec3 capsule_axis_;
        float degree;


        // init from lua
        void initFromLua(const sol::table &value) override {
            if (value["capsule_radius_"].valid())
                capsule_radius_ = value["capsule_radius_"];
            if (value["capsule_half_height_"].valid())
                capsule_half_height_ = value["capsule_half_height_"];
            if (value["degree"].valid())
                degree = value["degree"];
            if (value["capsule_axis_"].valid())
                capsule_axis_ = value["capsule_axis_"];
            if (value["state_"].valid())
                state = value["state_"];
            if (value["is_trigger_"].valid())
                is_trigger_ = value["is_trigger_"];

        }
        virtual sol::object getLuaValue(const sol::table &value, const std::string &key) override {
            if (key == "capsule_radius_") {
                return sol::make_object(value.lua_state(), capsule_radius_);
            }
            if (key == "capsule_half_height_") {
                return sol::make_object(value.lua_state(), capsule_half_height_);
            }
            if (key == "degree") {
                return sol::make_object(value.lua_state(), degree);
            }
            if (key == "capsule_axis_") {
                return sol::make_object(value.lua_state(), capsule_axis_);
            }
            if (key == "state_") {
                return sol::make_object(value.lua_state(), state);
            }
            if (key == "is_trigger_") {
                return sol::make_object(value.lua_state(), is_trigger_);
            }
            return sol::nil;
        }



    };



    class BoxCollider:public Collider{
        friend class PhysicSystem;

    public:
        glm::vec3 box_size_;

        // init from lua
        void initFromLua(const sol::table &value) override {
            if(value["box_size_"].valid())
                box_size_ = value["box_size_"];
            if (value["state_"].valid())
                state = value["state_"];
            if (value["is_trigger_"].valid())
                is_trigger_ = value["is_trigger_"];
        }

        virtual sol::object getLuaValue(const sol::table &value, const std::string &key) override {
            if (key == "box_size_") {
                return sol::make_object(value.lua_state(), box_size_);
            }
            if (key == "state_") {
                return sol::make_object(value.lua_state(), state);
            }
            if (key == "is_trigger_") {
                return sol::make_object(value.lua_state(), is_trigger_);
            }
            return sol::nil;
        }
    };


}

#endif //SPARROW_COLLIDER_H
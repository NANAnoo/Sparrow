//
// Created by duhao on 2023/4/1.
//

#ifndef SPARROW_SIMULATIONEVENTCALLBACK_H
#define SPARROW_SIMULATIONEVENTCALLBACK_H

#include <PxPhysicsAPI.h>
#include <iostream>
#include <functional>
#include "EcsFramework/System/PhysicSystem/PhysicSystem.h"
#include "EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/Component/PhysicalComponent/Collider.h"


using namespace physx;
namespace SPW {


    class SimulationEventCallback : public PxSimulationEventCallback {
    public:
        std::weak_ptr<entt::registry> weak_registry;
        void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 count) override {
            while (count--) {
                const PxContactPair &current = *pairs++;

                for (int i = 0; i < 2; ++i) {
                    PxShape *shape = current.shapes[i];
                    PxShape *another_shape = current.shapes[i ^ 1];


                    bool is_trigger = shape->getSimulationFilterData().word0 & 0x1;
                    if (is_trigger) {
                        continue;
                    }

                    Entity gameObject = SPW::Entity(static_cast<entt::entity>(int(shape->userData)), weak_registry.lock());

                    Entity other_gameObject = SPW::Entity(static_cast<entt::entity>(int(another_shape->userData)), weak_registry.lock());;


                    if (current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND)) {
                        SPW::Entity e = SPW::Entity(static_cast<entt::entity>(int(shape->userData)), weak_registry.lock());
                        if(e.has<SPW::BoxCollider>() && e.component<BoxCollider>()->contact_enter != nullptr){
                            e.component<BoxCollider>()->contact_enter(&gameObject,&other_gameObject);
                        }
                        if(e.has<SPW::SphereCollider>() && e.component<SphereCollider>()->contact_enter != nullptr){
                            e.component<SphereCollider>()->contact_enter(&gameObject,&other_gameObject);
                        }
                        if(e.has<SPW::CapsuleCollider>() && e.component<CapsuleCollider>()->contact_enter != nullptr){
                            e.component<CapsuleCollider>()->contact_enter(&gameObject,&other_gameObject);
                        }

                    }

                    if (current.events & (PxPairFlag::eNOTIFY_TOUCH_LOST)) {
                        SPW::Entity e = SPW::Entity(static_cast<entt::entity>(int(shape->userData)), weak_registry.lock());
                        if(e.has<SPW::BoxCollider>() && e.component<BoxCollider>()->contact_leave != nullptr){
                            e.component<BoxCollider>()->contact_leave(&gameObject,&other_gameObject);
                        }
                        if(e.has<SPW::SphereCollider>() && e.component<SphereCollider>()->contact_leave != nullptr){
                            e.component<SphereCollider>()->contact_leave(&gameObject,&other_gameObject);
                        }
                        if(e.has<SPW::CapsuleCollider>() && e.component<CapsuleCollider>()->contact_leave != nullptr){
                            e.component<CapsuleCollider>()->contact_leave(&gameObject,&other_gameObject);
                        }
                    }
                }
            }

        }

        void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count) override {

            while (count--) {
                const PxTriggerPair &current = *pairs++;
                if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                    PxShape *trigger_shape = current.triggerShape;
                    SPW::Entity gameObject = SPW::Entity(static_cast<entt::entity>(int(trigger_shape->userData)), weak_registry.lock());
                    if(gameObject.has<SPW::BoxCollider>()&&gameObject.component<BoxCollider>()->trigger_enter!= nullptr){
                        gameObject.component<BoxCollider>()->trigger_enter(&gameObject);
                    }
                    if(gameObject.has<SPW::SphereCollider>()&&gameObject.component<SphereCollider>()->trigger_enter!= nullptr){
                        gameObject.component<SphereCollider>()->trigger_enter(&gameObject);
                    }
                    if(gameObject.has<SPW::CapsuleCollider>()&&gameObject.component<CapsuleCollider>()->trigger_enter!= nullptr){
                        gameObject.component<CapsuleCollider>()->trigger_enter(&gameObject);
                    }
                }
                if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST) {
                    PxShape *trigger_shape = current.triggerShape;
                    Entity gameObject = SPW::Entity(static_cast<entt::entity>(int(trigger_shape->userData)), weak_registry.lock());
                    if(gameObject.has<SPW::BoxCollider>()&&gameObject.component<BoxCollider>()->trigger_leave!= nullptr){
                        gameObject.component<BoxCollider>()->trigger_leave(&gameObject);
                    }
                    if(gameObject.has<SPW::SphereCollider>()&&gameObject.component<SphereCollider>()->trigger_leave!= nullptr){
                        gameObject.component<SphereCollider>()->trigger_leave(&gameObject);
                    }
                    if(gameObject.has<SPW::CapsuleCollider>()&&gameObject.component<CapsuleCollider>()->trigger_leave!= nullptr){
                        gameObject.component<CapsuleCollider>()->trigger_leave(&gameObject);
                    }

                }
            }


        }

        void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) override {
            std::cout << "onConstraintBreak" << std::endl;
        }

        void onWake(PxActor **actors, PxU32 count) override {
            std::cout << "onWake" << std::endl;
        }

        void onSleep(PxActor **actors, PxU32 count) override {
            std::cout << "onSleep" << std::endl;
        }


        void onAdvance(const PxRigidBody *const *, const PxTransform *, const PxU32) override {

        }


    };
}

#endif //SPARROW_SIMULATIONEVENTCALLBACK_H
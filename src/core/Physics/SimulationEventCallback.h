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


    auto const myCallback = []() {


    };


    class SimulationEventCallback : public PxSimulationEventCallback {
    public:
        void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 count) override {

            SPW::myCallback();
            while (count--) {
                const PxContactPair &current = *pairs++;

                for (int i = 0; i < 2; ++i) {
                    PxShape *shape = current.shapes[i];
                    PxShape *another_shape = current.shapes[i ^ 1];


                    bool is_trigger = shape->getSimulationFilterData().word0 & 0x1;
                    if (!is_trigger) {
                        continue;
                    }

                    Entity *gameObject = static_cast<Entity *>(shape->userData);

                    Entity *other_gameObject = static_cast<Entity *>(another_shape->userData);


                    if (current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND)) {
                        SPW::Entity *e = static_cast<Entity *>(shape->userData);
                        if(e->has<SPW::Collider>()){
                            e->component<Collider>()->contact_enter(gameObject,other_gameObject);
                        }

                    }

                    if (current.events & (PxPairFlag::eNOTIFY_TOUCH_LOST)) {
                        SPW::Entity *e = static_cast<Entity *>(shape->userData);
                        if(e->has<SPW::Collider>()){
                            e->component<Collider>()->contact_leave(gameObject,other_gameObject);
                        }


                    }
                }
            }

        }

        void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count) override {

            while (count--) {
                const PxTriggerPair &current = *pairs++;
                if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                    PxShape *trigger_shape = pairs->triggerShape;
                    Entity *gameObject = static_cast<Entity *>(trigger_shape->userData);
                    if(gameObject->has<SPW::Collider>()){
                        gameObject->component<Collider>()->trigger_enter(gameObject);
                    }

                }
                if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST) {
                    PxShape *trigger_shape = pairs->triggerShape;
                    Entity *gameObject = static_cast<Entity *>(trigger_shape->userData);
                    if(gameObject->has<SPW::Collider>()){
                        gameObject->component<Collider>()->trigger_leave(gameObject);
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
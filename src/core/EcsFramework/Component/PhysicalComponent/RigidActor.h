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

namespace SPW{

    enum RigidState{Sleep,Awake,Awake2};


    class RigidStaticComponent : public ComponentI{
    public:
        physx::PxRigidActor* rigid;
        RigidState rigidState = Sleep;
    };

    class RigidDynamicComponent : public ComponentI{
    public:
        physx::PxRigidBody* rigid;
        RigidState rigidState = Sleep;
    private:
      bool enable_ccd_;
    };
};
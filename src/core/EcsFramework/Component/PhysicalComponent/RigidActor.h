//
// Created by duhao on 2023/3/27.
//

#ifndef SPARROW_RIGIDACTOR_H
#define SPARROW_RIGIDACTOR_H

#endif //SPARROW_RIGIDACTOR_H
#include "../ComponentI.h"
#include <PxPhysicsAPI.h>
#include <memory>

namespace SPW{

    enum RigidComponent{Sleep,Awake,WithCollider};

    class RigidActor : public ComponentI{
    public:
        RigidComponent rigidState = Sleep;
        std::shared_ptr<physx::PxRigidActor> rigid;
    protected:


    };
};
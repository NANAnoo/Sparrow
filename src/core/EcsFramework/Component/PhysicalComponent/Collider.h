//
// Created by duhao on 2023/3/27.
//

#ifndef SPARROW_COLLIDER_H
#define SPARROW_COLLIDER_H

#endif //SPARROW_COLLIDER_H
#include <PxPhysicsAPI.h>
#include <memory>
#include "../ComponentI.h"


namespace SPW{

    class Collider : public ComponentI{
    public:
        Collider()=default;
        ~Collider()=default;
        std::unique_ptr<physx::PxRigidActor> rigid;

    };
}
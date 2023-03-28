//
// Created by duhao on 2023/3/27.
//
#include "../ComponentI.h"
#include <PxPhysicsAPI.h>
#include <memory>
#include "RigidActor.h"

#ifndef SPARROW_RIGIDDYNAMICCOMPONENT_H
#define SPARROW_RIGIDDYNAMICCOMPONENT_H

#endif //SPARROW_RIGIDDYNAMICCOMPONENT_H

namespace SPW{

    class RigidDynamicComponent : public RigidActor{
    private:
        bool enable_ccd_;
    };
}
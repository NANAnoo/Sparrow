//
// Created by duhao on 2023/3/27.
//

#ifndef SPARROW_PHYSICSYSTEM_H
#define SPARROW_PHYSICSYSTEM_H
#include <glm/glm.hpp>
#include "../SystemI.h"
#include "Physics/physic_error_call_back.h"
#include <PxPhysicsAPI.h>

namespace SPW{
    class PhysicSystem : public SystemI {


    public:
        explicit PhysicSystem(std::shared_ptr<Scene> &scene);
        virtual ~PhysicSystem() = default;

        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;

        static PxScene* CreatePxScene();
        static PxRigidDynamic* CreateRigidDynamic(const glm::vec3& pos,const char* name);
        static PxRigidStatic* CreateRigidStatic(const glm::vec3& pos,const char* name);


    private:
        static physx::PxDefaultAllocator px_allocator_;
        static PhysicErrorCallback	    physic_error_callback_;
        //static SimulationEventCallback  simulation_event_callback_;
        static physx::PxFoundation *px_foundation_;
        static physx::PxPhysics *px_physics_;
        static physx::PxDefaultCpuDispatcher *px_cpu_dispatcher_;
        static physx::PxScene *px_scene_;
        static physx::PxPvd *px_pvd_;
    };

}





#endif //SPARROW_PHYSICSYSTEM_H

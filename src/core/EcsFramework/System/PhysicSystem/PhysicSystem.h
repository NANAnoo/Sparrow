//
// Created by duhao on 2023/3/27.
//

#ifndef SPARROW_PHYSICSYSTEM_H
#define SPARROW_PHYSICSYSTEM_H
#include <glm/glm.hpp>
#include "../SystemI.h"
#include "Physics/physic_error_call_back.h"
#include <PxPhysicsAPI.h>
#include <memory>

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

        PxScene* CreatePxScene();
        PxRigidDynamic* CreateRigidDynamic(const glm::vec3& pos,const char* name);
        PxRigidStatic* CreateRigidStatic(const glm::vec3& pos,const char* name);

        PxMaterial* CreatPhysicMaterial(float static_friction, float dynamic_friction, float restitution);

        //static bool AttachSphereCollider(float radius_, PxMaterial* material);
        //static bool AttachBoxCollider(glm::vec3 size_, PxMaterial* material);
        //static bool AttachCapsuleCollider(float capsule_radius_,float capsule_half_height_, PxMaterial* material);


    private:
        PxDefaultAllocator px_allocator_;
        PhysicErrorCallback	    physic_error_callback_;
        //static SimulationEventCallback  simulation_event_callback_;
        PxFoundation *px_foundation_;
        PxPhysics *px_physics_;
        PxDefaultCpuDispatcher *px_cpu_dispatcher_;
        PxScene *px_scene_;
        PxPvd *px_pvd_;
    };

}





#endif //SPARROW_PHYSICSYSTEM_H

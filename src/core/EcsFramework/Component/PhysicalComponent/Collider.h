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


        std::function<void(const SPW::Entity* e)> trigger_enter;
        std::function<void(const SPW::Entity* e)> trigger_leave;
        std::function<void(const SPW::Entity* e1,const SPW::Entity* e2)> contact_enter;
        std::function<void(const SPW::Entity* e1,const SPW::Entity* e2)> contact_leave;




    protected:

        physx::PxMaterial* px_material_;



    };

    class SphereCollider : public Collider{
        friend class PhysicSystem;
    public:
        float sphere_radius_;


    };


    class CapsuleCollider : public Collider{
        friend class PhysicSystem;
    public:

        float capsule_radius_;
        float capsule_half_height_;
        // Adjust capsule shape
        glm::vec3 capsule_axis_;
        float degree;

    };

    class BoxCollider:public Collider{
        friend class PhysicSystem;

    public:
        glm::vec3 box_size_;
    };


}

#endif //SPARROW_COLLIDER_H
//
// Created by duhao on 2023/3/27.
//
#include <PxPhysicsAPI.h>
#include "PhysicSystem.h"
#include "../SystemI.h"
#include "./EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/Scene.hpp"

#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/PhysicalComponent/RigidDynamicComponent.h"
#include "EcsFramework/Component/PhysicalComponent/RigidStaticComponent.h"


namespace SPW{

    PhysicSystem::PhysicSystem(std::shared_ptr<Scene> &scene) : SystemI(scene) {}

    void PhysicSystem::initial() {
        //Creates an instance of the foundation class,The foundation class is needed to initialize higher level SDKs.only one instance per process.
        px_foundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, px_allocator_, physic_error_callback_);

        //Connect to pvd(PhysX Visual Debugger).
        px_pvd_ = PxCreatePvd(*px_foundation_);
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        px_pvd_->connect(*transport,PxPvdInstrumentationFlag::eALL);

        //Creates an instance of the physics SDK.
        px_physics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *px_foundation_, PxTolerancesScale(),true,px_pvd_);

        px_scene_ = CreatePxScene();

    }




    void PhysicSystem::onUpdate(TimeDuration dt) {



            ComponentGroup<SPW::NameComponent, SPW::TransformComponent, SPW::RigidDynamicComponent> RigidDynamicGroup;
            locatedScene.lock()->forEachEntityInGroup(
                    RigidDynamicGroup,
                    [this](const Entity &en) {
                        auto nc = en.component<NameComponent>();
                        auto tc = en.component<TransformComponent>();
                        auto dc=en.component<RigidDynamicComponent>();
                        dc->rigid =;
                    });







        }











//create scene
    PxScene* PhysicSystem::CreatePxScene() {
        if(px_physics_==nullptr) {
            std::cout<<"px_physics_==nullptr,please call Physics::Initial() first"<<std::endl;
            return nullptr;
        }

        PxSceneDesc sceneDesc(px_physics_->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
        px_cpu_dispatcher_ = PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher	= px_cpu_dispatcher_;

        //sceneDesc.simulationEventCallback = &simulation_event_callback_;

        sceneDesc.filterShader	= PxDefaultSimulationFilterShader;

        PxScene* px_scene = px_physics_->createScene(sceneDesc);

        PxPvdSceneClient* pvd_client = px_scene->getScenePvdClient();

        if(pvd_client)
        {
            pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }

        return px_scene;
    }

    PxRigidDynamic* PhysicSystem::CreateRigidDynamic(const glm::vec3& pos,const char* name){
        PxRigidDynamic* rigid = px_physics_->createRigidDynamic(PxTransform(PxVec3(pos.x,pos.y,pos.z)));
        rigid->setName(name);
        px_scene_->addActor(*rigid);
        return rigid;
    }

    PxRigidStatic* PhysicSystem::CreateRigidStatic(const glm::vec3 &pos, const char *name) {
        PxRigidStatic* rigid = px_physics_->createRigidStatic(PxTransform(PxVec3(pos.x,pos.y,pos.z)));
        rigid->setName(name);
        px_scene_->addActor(*rigid);
        return rigid;
    }

}
//
// Created by duhao on 2023/3/27.
//
#include <PxPhysicsAPI.h>
#include "PhysicSystem.h"
#include "../SystemI.h"
#include "./EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/Scene.hpp"

#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/PhysicalComponent/RigidActor.h"
#include "EcsFramework/Component/PhysicalComponent/Collider.h"
#include <iostream>



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

    void PhysicSystem::beforeUpdate() {

        ComponentGroup<SPW::IDComponent, SPW::TransformComponent, SPW::RigidDynamicComponent> RigidDynamicGroup;
        locatedScene.lock()->forEachEntityInGroup(
                RigidDynamicGroup,
                [this](const Entity &en) {
                    auto nc = en.component<IDComponent>();
                    auto tc = en.component<TransformComponent>();
                    auto dc=en.component<RigidDynamicComponent>();

                    if(dc->rigidState==Awake&&dc->rigid== nullptr){
                        dc->rigid =(PhysicSystem::CreateRigidDynamic(tc->position,nc->getID().toString().c_str()));
                    }

                    //auto box_collider = en.component<SphereCollider>();
                    //auto capsule_collider = en.component<CapsuleCollider>();
                    if(en.has<SphereCollider>()){
                        auto sphere_collider = en.component<SphereCollider>();
                        if(sphere_collider->state==needAwake){
                            float radius = sphere_collider->sphere_radius_;

                            PxMaterial* material_temp;
                            if(sphere_collider->custom_material_){

                                auto custom_material = sphere_collider->custom_material_;
                                material_temp=CreatPhysicMaterial(custom_material->static_friction(),custom_material->static_friction(),custom_material->restitution());
                            }else{
                                material_temp=CreatPhysicMaterial(0.5,0.5,0.5);
                            }


                            //PxShape* shape = px_physics_->createShape(PxSphereGeometry(radius),*material_temp,true);
                            //dc->rigid->attachShape(*shape);
                            //shape->release();

                            PxRigidActorExt::createExclusiveShape(*dc->rigid,PxSphereGeometry(radius),*material_temp);



                            material_temp->release();
                            PxRigidBodyExt::updateMassAndInertia(*dc->rigid, 1.0f,false);
                            sphere_collider->state=awake;
                            std::cout<<"1"<<std::endl;
                        }

                    }

                    if(en.has<CapsuleCollider>()){
                        auto capsule_collider = en.component<CapsuleCollider>();
                        if(capsule_collider->state==needAwake){
                            float capsule_radius = capsule_collider->capsule_radius_;
                            float capsule_half_height = capsule_collider->capsule_half_height_;
                            PxMaterial* material_temp;
                            if(capsule_collider->custom_material_){

                                auto custom_material = capsule_collider->custom_material_;
                                material_temp=CreatPhysicMaterial(custom_material->static_friction(),custom_material->static_friction(),custom_material->restitution());
                            }else{
                                material_temp=CreatPhysicMaterial(0.5,0.5,0.5);
                            }



                            PxShape* shape = px_physics_->createShape(PxCapsuleGeometry(capsule_radius,capsule_half_height),*material_temp);
                            dc->rigid->attachShape(*shape);
                            shape->release();
                            material_temp->release();
                            PxRigidBodyExt::updateMassAndInertia(*dc->rigid, 1.0f,false);
                            capsule_collider->state=awake;
                            std::cout<<"2"<<std::endl;
                        }

                    }




                });


        ComponentGroup<SPW::IDComponent, SPW::TransformComponent, SPW::RigidStaticComponent> RigidStaticGroup;
        locatedScene.lock()->forEachEntityInGroup(
                RigidStaticGroup,
                [this](const Entity &en) {
                    auto nc = en.component<IDComponent>();
                    auto tc = en.component<TransformComponent>();
                    auto sc=en.component<RigidStaticComponent>();
                    if(sc->rigidState==Awake&&sc->rigid==nullptr){
                        sc->rigid =static_cast<PxRigidActor*>(PhysicSystem::CreateRigidStatic(tc->position,nc->getID().toString().c_str()));
                    }

                    if(en.has<CapsuleCollider>()){
                        auto capsule_collider = en.component<CapsuleCollider>();
                        if(capsule_collider->state==needAwake){
                            float capsule_radius = capsule_collider->capsule_radius_;
                            float capsule_half_height = capsule_collider->capsule_half_height_;
                            PxMaterial* material_temp;
                            if(capsule_collider->custom_material_){

                                auto custom_material = capsule_collider->custom_material_;
                                material_temp=CreatPhysicMaterial(custom_material->static_friction(),custom_material->static_friction(),custom_material->restitution());
                            }else{
                                material_temp=CreatPhysicMaterial(0.5,0.5,0.5);
                            }



                            PxShape* shape = px_physics_->createShape(PxCapsuleGeometry(capsule_radius,capsule_half_height),*material_temp);
                            sc->rigid->attachShape(*shape);
                            shape->release();
                            material_temp->release();
                            //PxRigidBodyExt::updateMassAndInertia(*sc->rigid, 1.0f,false);
                            capsule_collider->state=awake;
                            std::cout<<"2"<<std::endl;
                        }

                    }



                    if(en.has<BoxCollider>()){
                        auto box_collider = en.component<BoxCollider>();
                        if(box_collider->state==needAwake){
                            glm::vec3 box_radius = box_collider->box_size_;

                            PxMaterial* material_temp;
                            if(box_collider->custom_material_){

                                auto custom_material = box_collider->custom_material_;
                                material_temp=CreatPhysicMaterial(custom_material->static_friction(),custom_material->static_friction(),custom_material->restitution());
                            }else{
                                material_temp=CreatPhysicMaterial(0.5,0.5,0.5);
                            }



                            PxShape* shape = px_physics_->createShape(PxBoxGeometry(box_radius.x,box_radius.y,box_radius.z),*material_temp);
                            sc->rigid->attachShape(*shape);
                            shape->release();
                            material_temp->release();
                            //PxRigidBodyExt::updateMassAndInertia(*sc->rigid, 1.0f,false);
                            box_collider->state=awake;
                            std::cout<<"2"<<std::endl;
                        }

                    }


                });







    }

    void PhysicSystem::onUpdate(TimeDuration dt) {

        //creat dynamic rigid









    }

    void PhysicSystem::afterUpdate() {
        px_scene_->simulate(1.0f/60.0f);
        px_scene_ ->fetchResults(true);
    }





    void PhysicSystem::onStop() {}











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
            std::cout<<"set up pvd_Client successfully "<<std::endl;

            pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }
        std::cout<<px_scene<<std::endl;
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


    PxMaterial* PhysicSystem::CreatPhysicMaterial(float static_friction, float dynamic_friction, float restitution){
        PxMaterial* material = px_physics_->createMaterial(static_friction,dynamic_friction,restitution);
        return material;
    }

}
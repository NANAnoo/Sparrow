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



    static PxFilterFlags SimulationFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,PxFilterObjectAttributes attributes1, PxFilterData filterData1,PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {
        pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND;
        return PxFilterFlags();
    }








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

                            if(sphere_collider->is_trigger_){
                                PxShape* sphere_shape = PxRigidActorExt::createExclusiveShape(*dc->rigid,PxSphereGeometry(radius),*material_temp,physx::PxShapeFlag::eVISUALIZATION|physx::PxShapeFlag::eTRIGGER_SHAPE);
                                sphere_shape->userData = (void*)&en;
                            }else{
                                PxShape* sphere_shape = PxRigidActorExt::createExclusiveShape(*dc->rigid,PxSphereGeometry(radius),*material_temp);
                                sphere_shape->userData = (void*)&en;
                            }


                            material_temp->release();
                            PxRigidBodyExt::updateMassAndInertia(*dc->rigid, 1.0f,false);
                            sphere_collider->state=awake;
                        }

                    }

                    if(en.has<CapsuleCollider>()){
                        auto capsule_collider = en.component<CapsuleCollider>();
                        if(capsule_collider->state==needAwake){
                            float capsule_radius = capsule_collider->capsule_radius_;
                            float capsule_half_height = capsule_collider->capsule_half_height_;
                            float degree =capsule_collider->degree;
                            glm::vec3 axis = capsule_collider->capsule_axis_;


                            PxMaterial* material_temp;
                            if(capsule_collider->custom_material_){
                                auto custom_material = capsule_collider->custom_material_;
                                material_temp=CreatPhysicMaterial(custom_material->static_friction(),custom_material->static_friction(),custom_material->restitution());
                            }else{
                                material_temp=CreatPhysicMaterial(0.5,0.5,0.5);
                            }

                            if(capsule_collider->is_trigger_){
                                PxTransform relativePose(PxQuat(degree, PxVec3(axis.x,axis.y,axis.z)));
                                PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*dc->rigid,
                                                                                               PxCapsuleGeometry(capsule_radius, capsule_half_height), *material_temp,physx::PxShapeFlag::eVISUALIZATION|physx::PxShapeFlag::eTRIGGER_SHAPE);

                                aCapsuleShape->setLocalPose(relativePose);
                                aCapsuleShape->userData = (void*)&en;

                            }else{
                                PxTransform relativePose(PxQuat(degree, PxVec3(axis.x,axis.y,axis.z)));
                                PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*dc->rigid,
                                                                                               PxCapsuleGeometry(capsule_radius, capsule_half_height), *material_temp);
                                aCapsuleShape->setLocalPose(relativePose);
                                aCapsuleShape->userData = (void*)&en;

                            }

                            material_temp->release();
                            PxRigidBodyExt::updateMassAndInertia(*dc->rigid, 1.0f,false);
                            capsule_collider->state=awake;
                        }

                    }

                    if(en.has<BoxCollider>()){
                        auto box_collider = en.component<BoxCollider>();
                        if(box_collider->state==needAwake){
                            glm::vec3 box_size = box_collider->box_size_;

                            PxMaterial* material_temp;
                            if(box_collider->custom_material_){
                                auto custom_material = box_collider->custom_material_;
                                material_temp=CreatPhysicMaterial(custom_material->static_friction(),custom_material->static_friction(),custom_material->restitution());
                            }else{
                                material_temp=CreatPhysicMaterial(0.5,0.5,0.5);
                            }

                            if(box_collider->is_trigger_){
                                PxShape* box_shape = PxRigidActorExt::createExclusiveShape(*dc->rigid,PxBoxGeometry(box_size.x,box_size.y,box_size.z),*material_temp,physx::PxShapeFlag::eVISUALIZATION|physx::PxShapeFlag::eTRIGGER_SHAPE);
                                box_shape->userData = (void*)&en;
                            }else{
                                PxShape* box_shape = PxRigidActorExt::createExclusiveShape(*dc->rigid,PxBoxGeometry(box_size.x,box_size.y,box_size.z),*material_temp);
                                box_shape->userData = (void*)&en;
                            }


                            material_temp->release();
                            PxRigidBodyExt::updateMassAndInertia(*dc->rigid, 1.0f,false);
                            box_collider->state=awake;
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
                            float degree =capsule_collider->degree;
                            glm::vec3 axis = capsule_collider->capsule_axis_;
                            PxMaterial* material_temp;
                            if(capsule_collider->custom_material_){

                                auto custom_material = capsule_collider->custom_material_;
                                material_temp=CreatPhysicMaterial(custom_material->static_friction(),custom_material->static_friction(),custom_material->restitution());
                            }else{
                                material_temp=CreatPhysicMaterial(0.5,0.5,0.5);
                            }



                            if(capsule_collider->is_trigger_){
                                PxTransform relativePose(PxQuat(degree, PxVec3(axis.x,axis.y,axis.z)));
                                PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*sc->rigid,
                                                                                               PxCapsuleGeometry(capsule_radius, capsule_half_height), *material_temp,physx::PxShapeFlag::eVISUALIZATION|physx::PxShapeFlag::eTRIGGER_SHAPE);

                                aCapsuleShape->setLocalPose(relativePose);
                                aCapsuleShape->userData = (void*)&en;

                            }else{
                                PxTransform relativePose(PxQuat(degree, PxVec3(axis.x,axis.y,axis.z)));
                                PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*sc->rigid,
                                                                                               PxCapsuleGeometry(capsule_radius, capsule_half_height), *material_temp);
                                aCapsuleShape->setLocalPose(relativePose);
                                aCapsuleShape->userData = (void*)&en;

                            }



                            material_temp->release();

                            capsule_collider->state=awake;
                        }

                    }



                    if(en.has<BoxCollider>()){
                        auto box_collider = en.component<BoxCollider>();
                        if(box_collider->state==needAwake){
                            glm::vec3 box_size = box_collider->box_size_;

                            PxMaterial* material_temp;
                            if(box_collider->custom_material_){

                                auto custom_material = box_collider->custom_material_;
                                material_temp=CreatPhysicMaterial(custom_material->static_friction(),custom_material->static_friction(),custom_material->restitution());
                            }else{
                                material_temp=CreatPhysicMaterial(0.5,0.5,0.5);
                            }


                            if(box_collider->is_trigger_){
                                PxShape* box_shape = PxRigidActorExt::createExclusiveShape(*sc->rigid,PxBoxGeometry(box_size.x,box_size.y,box_size.z),*material_temp,physx::PxShapeFlag::eVISUALIZATION|physx::PxShapeFlag::eTRIGGER_SHAPE);
                                box_shape->userData = (void*)&en;
                            }else{
                                PxShape* box_shape = PxRigidActorExt::createExclusiveShape(*sc->rigid,PxBoxGeometry(box_size.x,box_size.y,box_size.z),*material_temp);
                                box_shape->userData = (void*)&en;
                            }
                            material_temp->release();

                            box_collider->state=awake;

                        }

                    }

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

                            if(sphere_collider->is_trigger_){
                                PxShape* sphere_shape = PxRigidActorExt::createExclusiveShape(*sc->rigid,PxSphereGeometry(radius),*material_temp,physx::PxShapeFlag::eVISUALIZATION|physx::PxShapeFlag::eTRIGGER_SHAPE);
                                sphere_shape->userData = (void*)&en;
                            }else{
                                PxShape* sphere_shape = PxRigidActorExt::createExclusiveShape(*sc->rigid,PxSphereGeometry(radius),*material_temp);
                                sphere_shape->userData = (void*)&en;
                            }


                            material_temp->release();

                            sphere_collider->state=awake;
                        }

                    }
                });







    }

    void PhysicSystem::onUpdate(TimeDuration dt) {





    }

    void PhysicSystem::afterUpdate() {
        px_scene_->simulate(1.0f/60.0f);
        px_scene_ ->fetchResults(true);

        ComponentGroup<SPW::IDComponent, SPW::TransformComponent, SPW::RigidDynamicComponent> RigidDynamicGroup;
        locatedScene.lock()->forEachEntityInGroup(
                RigidDynamicGroup,
                [this](const Entity &en) {
                    auto nc = en.component<IDComponent>();
                    auto tc = en.component<TransformComponent>();
                    auto dc=en.component<RigidDynamicComponent>();

                    if(dc->rigidState==Awake&&dc->rigid != nullptr){
                        tc->position = glm::vec3 (dc->rigid->getGlobalPose().p.x,dc->rigid->getGlobalPose().p.y,dc->rigid->getGlobalPose().p.z);
                        tc->rotation = glm::vec3 (dc->rigid->getGlobalPose().q.x,dc->rigid->getGlobalPose().q.y,dc->rigid->getGlobalPose().q.z);
                    }

                });






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

        sceneDesc.simulationEventCallback = &simulation_event_callback_;

        sceneDesc.filterShader	= SimulationFilterShader;

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
#pragma once
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/Entity/Entity.hpp"
#include "Utils/UUID.hpp"
#include <sol/sol.hpp>
#include "ComponentWrapper.hpp"

#include <string>
#include <memory.h>

#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/BasicComponent/NameComponent.h"
#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/PhysicalComponent/RigidActor.h"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/AnimationComponent/AnimationComponent.h"

namespace SPW {
    class EntityWrapper{
        public:
        EntityWrapper(const std::shared_ptr<Entity> &aEntity)
            : m_entity(aEntity) {

        }

        EntityWrapper(const EntityWrapper&) = delete;
        EntityWrapper& operator=(const EntityWrapper&) = delete;

        EntityWrapper(EntityWrapper &&other) 
            : m_entity(std::move(other.m_entity)) {
        }
        
        EntityWrapper& operator=(EntityWrapper &&other) {
            m_entity = std::move(other.m_entity);
            return *this;
        }
        
        // @lua, getID
        std::string getID() {
            return m_entity->getUUID().toString();
        }

        // @lua, getName
        std::string getName() {
            return m_entity->getName();
        }

        // check if the entity is valid
        bool isValid() {
            return m_entity != nullptr;
        }

        // @lua, get ComponentWrapper by type
        ComponentWrapper getComponent(const std::string &type) {
            // create a component wrapper based on type
            if (type == "Transform") {
                return ComponentWrapper::createCompWrapper<TransformComponent>(m_entity);
            } else if (type == "MeshComponent") {
                return ComponentWrapper::createCompWrapper<MeshComponent>(m_entity);
            } else if (type == "PerspectiveCamera") {
                auto wrapper = ComponentWrapper::createCompWrapper<CameraComponent>(m_entity);
                if (wrapper.isValid() && m_entity->component<CameraComponent>()->getType() != CameraType::PerspectiveType) {
                    return wrapper;
                }
            } else if (type == "OrthoCamera") {
                auto wrapper = ComponentWrapper::createCompWrapper<CameraComponent>(m_entity);
                if (wrapper.isValid()) {
                    return wrapper;
                }
            } else if (type == "DirectionalLight") {
                return ComponentWrapper::createCompWrapper<DirectionalLightComponent>(m_entity);
            } else if (type == "PointLight") {
                return ComponentWrapper::createCompWrapper<PointLightComponent>(m_entity);
            } else if (type == "KeyEventHandler") {
                return ComponentWrapper::createCompWrapper<KeyComponent>(m_entity);
            } else if (type == "MouseEventHandler") {
                return ComponentWrapper::createCompWrapper<MouseComponent>(m_entity);
            } else if (type == "AudioComponent") {
                return ComponentWrapper::createCompWrapper<AudioComponent>(m_entity);
            } else if (type == "AudioListener") {
                return ComponentWrapper::createCompWrapper<AudioListener>(m_entity);
            } else if(type == "AnimationComponent") {
                return ComponentWrapper::createCompWrapper<AnimationComponent>(m_entity);
            } else if (type == "RigidDynamicComponent"){
                return ComponentWrapper::createCompWrapper<RigidDynamicComponent>(m_entity);
            }else if (type == "Collider"){
                return ComponentWrapper::createCompWrapper<Collider>(m_entity);
            } else if (type == "SphereCollider") {
                return ComponentWrapper::createCompWrapper<SphereCollider>(m_entity);
            } else if (type == "CapsuleCollider") {
                return ComponentWrapper::createCompWrapper<CapsuleCollider>(m_entity);
            } else if (type == "BoxCollider") {
                return ComponentWrapper::createCompWrapper<BoxCollider>(m_entity);
            }else if (type == "RigidStaticComponent"){
                return ComponentWrapper::createCompWrapper<RigidStaticComponent>(m_entity);
            }
            return ComponentWrapper(m_entity);
        }


        // @lua, createComponent
        ComponentWrapper createComponent(const std::string &type, const sol::table &value) {
            auto res = ComponentWrapper(m_entity);
            if (type == "Transform") {
                res.setupComponent<TransformComponent>(value);
            } else if (type == "MeshComponent") {
                std::string camera_id = value["cam_id"];
                UUID cam = UUID::fromString(camera_id.c_str());
                res.setupComponent<MeshComponent>(value, cam);
            } else if (type == "PerspectiveCamera") {
                res.setupComponent<CameraComponent>(value, CameraType::PerspectiveType);
            } else if (type == "OrthoCamera") {
                res.setupComponent<CameraComponent>(value, CameraType::OrthoType);
            } else if (type == "DirectionalLight") {
                res.setupComponent<DirectionalLightComponent>(value);
            } else if (type == "PointLight") {
                res.setupComponent<PointLightComponent>(value);
            } else if (type == "KeyEventHandler") {
                res.setupComponent<KeyComponent>(value);
            } else if (type == "MouseEventHandler") {
                res.setupComponent<MouseComponent>(value);
            } else if (type == "AudioComponent") {
                res.setupComponent<AudioComponent>(value);
            } else if (type == "AudioListener") {
                res.setupComponent<AudioListener>(value);
            } else if(type == "AnimationComponent"){
                res.setupComponent<AnimationComponent>(value);
            } else if (type == "RigidDynamicComponent"){
                res.setupComponent<RigidDynamicComponent>(value);
            } else if (type == "Collider"){
                res.setupComponent<Collider>(value);
            } else if (type == "SphereCollider"){
                res.setupComponent<SphereCollider>(value);
            } else if (type == "CapsuleCollider"){
                res.setupComponent<CapsuleCollider>(value);
            } else if (type == "BoxCollider"){
                res.setupComponent<BoxCollider>(value);
            } else if (type == "RigidStaticComponent"){
                res.setupComponent<RigidStaticComponent>(value);
            }
            return res;
        }

        static void bindLuaTable(sol::table &parent) {
            parent.new_usertype<EntityWrapper>("EntityWrapper",
                "getID", &EntityWrapper::getID,
                "createComponent", &EntityWrapper::createComponent,
                "getComponent", &EntityWrapper::getComponent,
                "isValid", &EntityWrapper::isValid,
                "getName", &EntityWrapper::getName);
        }

        private:
        std::shared_ptr<Entity> m_entity;
    };
}
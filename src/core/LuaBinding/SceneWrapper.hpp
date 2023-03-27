#pragma once
#include "EntityWrapper.hpp"
#include "EcsFramework/Scene.hpp"
#include "Utils/UUID.hpp"

#include <string>
#include <unordered_map>
#include <sol/sol.hpp>


namespace SPW {
    class SceneWrapper{
        public:
        SceneWrapper() = delete;

        SceneWrapper(const SceneWrapper&) = delete;
        SceneWrapper& operator=(const SceneWrapper&) = delete;

        SceneWrapper(SceneWrapper &&other) {
            m_scene = std::move(other.m_scene);
        }
        
        SceneWrapper& operator=(SceneWrapper &&other) {
            m_scene = std::move(other.m_scene);
            return *this;
        }

        // TODO: load a scene from path
        SceneWrapper(std::shared_ptr<EventResponderI> parent, std::string path) 
         : m_scene(std::make_shared<Scene>(parent)) {
        }

        // load from an existing scene
        SceneWrapper(const std::shared_ptr<Scene> &scene) 
         : m_scene(scene) {
        }

        // create entity from lua
        EntityWrapper createEntity(const std::string &aName) {
            auto newEntity = m_scene->createEntity(aName);
            auto id = newEntity->getUUID().toString();
            return EntityWrapper(newEntity);
        }

        // create entity wrapper from id
        EntityWrapper getEntityByID(const std::string &id) {
            auto entity = m_scene->getEntityByID(id);
            return EntityWrapper(entity);
        }

        // remove entity from lua
        void remove(const std::string &id) {
            m_scene->removeEntityByID(id);
        }

        // void initialize
        void initial() {
            m_scene->initial();
        }

        // before update
        void beforeUpdate() {
            m_scene->beforeUpdate();
        }

        // update
        void onUpdate(double dt) {
            m_scene->onUpdate(TimeDuration::second(dt));
        }

        // after update
        void afterUpdate() {
            m_scene->afterUpdate();
        }

        // stop
        void onStop() {
            m_scene->onStop();
        }

        static void bindLuaTable(sol::table &parent) {
            parent.new_usertype<SceneWrapper>("SceneWrapper",
                "createEntity", &SPW::SceneWrapper::createEntity,
                "remove", &SceneWrapper::remove,
                "initial", &SceneWrapper::initial,
                "beforeUpdate", &SceneWrapper::beforeUpdate,
                "onUpdate", &SceneWrapper::onUpdate,
                "afterUpdate", &SceneWrapper::afterUpdate,
                "onStop", &SceneWrapper::onStop,
                "getEntityByID", &SceneWrapper::getEntityByID);
        }

        std::shared_ptr<Scene> m_scene;
    };
}
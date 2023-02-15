//
// Created by ZhangHao on 2023/2/13.
//
#pragma once

#include "Event/Event.h"
#include "entt.hpp"
#include "Utils/Timer.hpp"
#include "Entity/Entity.h"
#include "System/SystemI.h"

#include <vector>

namespace SPW {
    // contains all entities, .eg Game world ...
    class Scene : public EventResponderI {
    public:
        explicit Scene(std::shared_ptr<EventResponderI> &parent)
            : EventResponderI(parent) {

        }
        // create scene with this line
        static std::shared_ptr<Scene> create(std::shared_ptr<EventResponderI> &parent) {
            auto scene = std::make_shared<Scene>(parent);
            return scene;
        }

        // create new entity in scene
        static std::shared_ptr<Entity> createEntity(const std::shared_ptr<Scene> &scene,
                                              const std::string &name,
                                              const UUID &uid) {
            auto ent = std::make_shared<Entity>(scene);
            ent->emplace<IDComponent>(uid);
            ent->emplace<NameComponent>(name);
            return ent;
        }

        static std::shared_ptr<Entity> createEntity(const std::shared_ptr<Scene> &scene,
                                              const std::string &name) {
            return createEntity(scene, name, UUID::randomUUID());
        }

        // delete entity
        void deleteEntity(const std::shared_ptr<Entity> &entity) {
            registry->destroy(entity->entity);
        }

        // for each
        template<Component ...C>
        static void forEach(const std::shared_ptr<Scene> &scene,
                            const std::function<void(const Entity &e)> &func) {
            for (auto &view : scene->getEntitiesWith<C...>(scene)) {
                func({view, scene});
            }
        }

        virtual void initial() {
            for (auto &system : systems) {
                system->initial();
            }
        }
        virtual void beforeUpdate() {
            for (auto &system : systems) {
                system->beforeUpdate();
            }
        };
        virtual void onUpdate(TimeDuration dt) {
            for (auto &system : systems) {
                system->onUpdate(dt);
            }
        };
        virtual void afterUpdate() {
            for (auto &system : systems) {
                system->afterUpdate();
            }
        };
        virtual void onStop() {
            for (auto &system : systems) {
                system->onStop();
            }
        };

    private:
        // get entity with components
        template<Component ...C>
        auto getEntitiesWith(const std::shared_ptr<Scene> &scene) {
            return registry->view<std::shared_ptr<C>...>();
        }

        std::shared_ptr<entt::registry> registry;
        std::vector<std::shared_ptr<SystemI>> systems;
        friend Entity;
    };
}


//
// Created by ZhangHao on 2023/2/14.
//

#ifndef SPARROW_ENTITY_HPP
#define SPARROW_ENTITY_HPP

#include <entt.hpp>
#include <memory>
#include "../Component/BasicComponent/IDComponent.h"
#include "../Component/BasicComponent/NameComponent.h"
#include "Utils/UUID.hpp"

namespace SPW {
    class Entity {
    public:
        Entity() = delete;
        explicit Entity(const std::shared_ptr<entt::registry> &located_registry) :
            registry(located_registry) {
            entity = located_registry->create();
        }
        Entity(const entt::entity &e, const std::shared_ptr<entt::registry> &located_registry):
            registry(located_registry), entity(e) {}
        Entity(const Entity &other) {
            registry = other.registry;
            entity = other.entity;
        };

        // insert a component
        template<Component C, typename ...Args>
        C *emplace(Args&& ...args) const {
            // check validation of weak_scene
            assert(!registry.expired());
            // add component to the registry
            return &registry.lock()->emplace<C>(entity, std::forward<Args>(args)...);
        }

        // get a component with type C
        template<Component C>
        [[nodiscard]] C * component() const {
            // check validation of weak_scene
            assert(!registry.expired());
            return &registry.lock()->get<C>(entity);
        }

        // get a tuple of components with selected types
        template<Component ...C>
        std::tuple<C *...> combined() const {
            return std::make_tuple<C *...>((component<C>())...);
        }
        template<Component ...C>
        std::tuple<C *...> combinedInGroup(ComponentGroup<C...>) const {
            return combined<C...>();
        }

        // check if a component with type C is existed in this entity
        template<Component C>
        bool has() const{
            // check validation of weak_scene
            assert(!registry.expired());
            return registry.lock()->all_of<C>(entity);
        }

        // remove the component with type C
        template<Component C>
        void remove() const {
            // check validation of weak_scene
            if (!registry.expired())
                registry.lock()->remove<C>(entity);
        }

        UUID getUUID() const{
            // check validation of weak_scene
            if (!registry.expired())
                return component<SPW::IDComponent>()->getID();
            else
                return SPW::UUID::noneID();
        }
        const std::string getName() {
            // check validation of weak_scene
            if (!registry.expired())
                return component<SPW::NameComponent>()->getName();
            else
                return "";
        }

        static Entity nullEntity() {
            return Entity(entt::null, nullptr);
        }

        bool isNull() const {
            return entity == entt::null;
        }

        bool operator==(const Entity& other) const
        {
            return entity == other.entity && registry.lock().get() == other.registry.lock().get();
        }

        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }
        entt::entity entity = {entt::null};
    private:
        std::weak_ptr<entt::registry> registry;
    };
}

#endif //SPARROW_ENTITY_HPP

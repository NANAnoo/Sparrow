//
// Created by ZhangHao on 2023/2/14.
//

#ifndef SPARROW_ENTITY_H
#define SPARROW_ENTITY_H

#include <entt.hpp>
#include <memory>
#include "../Component/ComponentI.h"
#include "Utils/UUID.hpp"

namespace SPW {
    class Scene;
    class Entity {
    public:
        Entity() = delete;
        explicit Entity(const std::shared_ptr<Scene> &located_scene);
        Entity(const entt::entity &e, const std::shared_ptr<Scene> &located_scene):
            scene(located_scene), entity(e) {}
        Entity(const Entity &other) = default;

        // insert a component
        template<Component C, typename ...Args>
        std::shared_ptr<C> emplace(Args ...args);

        // get a component with type C
        template<Component C>
        std::shared_ptr<C> component();

        // get a tuple of components with selected types
        template<Component ...C>
        std::tuple<const std::shared_ptr<C>...> combined() {
            return std::make_tuple<std::shared_ptr<C>...>(component<C>()...);
        }

        // check if a component with type C is existed in this entity
        template<Component C>
        bool has();

        // remove the component with type C
        template<Component C>
        void remove();

        UUID getUUID();
        const std::string& getName();

        bool operator==(const Entity& other) const
        {
            return entity == other.entity && scene.lock().get() == other.scene.lock().get();
        }

        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }

    private:
        entt::entity entity = {entt::null};
        std::weak_ptr<Scene> scene;
        friend Scene;
    };
}

#endif //SPARROW_ENTITY_H

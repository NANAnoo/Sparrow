//
// Created by ZhangHao on 2023/2/14.
//

#ifndef SPARROW_ENTITY_H
#define SPARROW_ENTITY_H

#include <entt.hpp>
#include <memory>

namespace SPW {
    class Scene;
    class Entity {
    public:
        Entity() = delete;


    private:
        entt::entity entity = {entt::null};
        std::weak_ptr<Scene> scene;
    };
}

#endif //SPARROW_ENTITY_H

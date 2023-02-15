//
// Created by ZhangHao on 2023/2/14.
//

#include "Entity.h"

#include "../Component/BasicComponent/IDComponent.h"
#include "../Component/BasicComponent/NameComponent.h"

#include "../Scene.hpp"

SPW::Entity::Entity(const std::shared_ptr<Scene> &located_scene):
    scene(located_scene) {
    entity = located_scene->registry->create();
}

template<SPW::Component C, typename ...Args>
std::shared_ptr<C> SPW::Entity::emplace(Args ...args) {
    // check validation of weak_scene
    if (scene.expired())
        return nullptr;
    // build component with arguments
    auto com = std::make_shared<C>(std::forward<Args>(args)...);
    // add component to the registry
    scene.lock()->registry->emplace<std::shared_ptr<C>>(entity, com);
    return com;
}

template<SPW::Component C>
std::shared_ptr<C> SPW::Entity::component() {
    // check validation of weak_scene
    if (scene.expired())
        return nullptr;
    return scene.lock()->registry->get<std::shared_ptr<C>>(entity);
}

template<SPW::Component C>
bool SPW::Entity::has() {
    // check validation of weak_scene
    if (scene.expired())
        return false;
    return scene.lock()->registry->all_of<std::shared_ptr<C>>(entity);
}

template<SPW::Component C>
void SPW::Entity::remove() {
    // check validation of weak_scene
    if (!scene.expired())
        scene.lock()->registry->remove<std::shared_ptr<C>>(entity);
}

SPW::UUID SPW::Entity::getUUID() {
    // check validation of weak_scene
    if (!scene.expired())
        return component<SPW::IDComponent>()->getID();
    else
        return SPW::UUID::noneID();
}

const std::string &SPW::Entity::getName() {
// check validation of weak_scene
    if (!scene.expired())
        return component<SPW::NameComponent>()->getName();
    else
        return NameComponent().getName();
}
//
// Created by ZhangHao on 2023/2/13.
//
#pragma once

#include "Event/Event.h"
#include "entt.hpp"

namespace SPW {
    // contains all entities, .eg Game world ...
    class Scene : public EventResponderI {
    public:
        explicit Scene(std::shared_ptr<EventResponderI> &parent)
            : EventResponderI(parent) {

        }
    private:
        entt::registry registry;
    };
}


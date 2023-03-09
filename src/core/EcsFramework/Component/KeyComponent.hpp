//
// Created by 29134 on 2023/2/24.
//
#pragma once
#include "ComponentI.h"
#include <functional>

#include "../Entity/Entity.hpp"
#include "Control/KeyCodes.h"
namespace SPW {

    class KeyComponent : public ComponentI {
    public:
        std::function<void(const SPW::Entity& e, KeyCode keycode)> onKeyDownCallBack;
        std::function<void(const SPW::Entity& e, KeyCode keycode)> onKeyHeldCallBack;
        std::function<void(const SPW::Entity& e, KeyCode keycode)> onKeyReleasedCallBack;
    };

}
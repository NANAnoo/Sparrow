//
// Created by 29134 on 2023/2/24.
//
#pragma once
#include "ComponentI.h"

namespace SPW {

    class KeyComponent : public ComponentI {
    public:
        std::function<void(const SPW::Entity& e, int keycode)> onKeyDownCallBack;
        std::function<void(const SPW::Entity& e, int keycode)> onKeyHeldCallBack;
        std::function<void(const SPW::Entity& e, int keycode)> onKeyReleasedCallBack;
    };

}
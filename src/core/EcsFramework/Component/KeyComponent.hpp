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

        void initFromLua(const sol::table &value) override {
            if (value["onKeyDown"].valid()) {
                onKeyDownCallBack = [value](const SPW::Entity& e, KeyCode keycode) {
                    value["onKeyDown"](e.getUUID().toString(), int(keycode));
                };
            }
            if (value["onKeyHeld"].valid()) {
                onKeyHeldCallBack = [value](const SPW::Entity& e, KeyCode keycode) {
                    value["onKeyHeld"](e.getUUID().toString(), int(keycode));
                };
            }
            if (value["onKeyUp"].valid()) {
                onKeyReleasedCallBack = [value](const SPW::Entity& e, KeyCode keycode) {
                    value["onKeyUp"](e.getUUID().toString(), int(keycode));
                };
            }
        }

        // update from lua
        void update(const std::string &key, const sol::table &value) final {
            if (!value["value"].valid()) return;
            sol:sol::protected_function callback = value["value"];
            if (key == "onKeyDown") {
                onKeyDownCallBack = [callback](const SPW::Entity& e, KeyCode keycode) {
                    callback(e.getUUID().toString(), int(keycode));
                };
            }
            if (key == "onKeyHeld") {
                onKeyHeldCallBack = [callback](const SPW::Entity& e, KeyCode keycode) {
                    callback(e.getUUID().toString(), int(keycode));
                };
            }
            if (key == "onKeyUp") {
                onKeyReleasedCallBack = [callback](const SPW::Entity& e, KeyCode keycode) {
                    callback(e.getUUID().toString(), int(keycode));
                };
            }
        }
    };

}
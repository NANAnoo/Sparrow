//
// Created by 29134 on 2023/2/26.
//
#pragma once
#include "ComponentI.h"
#include "Control/MouseCodes.h"
#include "EcsFramework/Entity/Entity.hpp"

namespace SPW {

    class MouseComponent : public ComponentI {
    public:
        std::function<void(const SPW::Entity& e, MouseCode button_code)> onMouseDownCallBack;
        std::function<void(const SPW::Entity& e, MouseCode button_code)> onMouseHeldCallBack;
        std::function<void(const SPW::Entity& e, MouseCode button_code)> onMouseReleasedCallBack;
        std::function<void(const SPW::Entity& e, double scroll_offset)> onMouseScrollCallBack;
        std::function<void(const SPW::Entity& e, double cursor_x, double cursor_y, double cursor_X_bias, double cursor_Y_bias)> cursorMovementCallBack;
        
        // init from lua
        void initFromLua(const sol::table &value) override {
            if (value["onMouseDown"].valid()) {
                onMouseDownCallBack = [value](const SPW::Entity& e, MouseCode button_code) {
                    value["onMouseDown"](e.getUUID().toString(), int(button_code));
                };
            }
            if (value["onMouseHeld"].valid()) {
                onMouseHeldCallBack = [value](const SPW::Entity& e, MouseCode button_code) {
                    value["onMouseHeld"](e.getUUID().toString(), int(button_code));
                };
            }
            if (value["onMouseUp"].valid()) {
                onMouseReleasedCallBack = [value](const SPW::Entity& e, MouseCode button_code) {
                    value["onMouseUp"](e.getUUID().toString(), int(button_code));
                };
            }
            if (value["onScroll"].valid()) {
                onMouseScrollCallBack = [value](const SPW::Entity& e, double scroll_offset) {
                    value["onScroll"](e.getUUID().toString(), scroll_offset);
                };
            }
            if (value["onMove"].valid()) {
                cursorMovementCallBack = [value](const SPW::Entity& e, double cursor_x, double cursor_y, double cursor_X_bias, double cursor_Y_bias) {
                    value["onMove"](e.getUUID().toString(), cursor_x, cursor_y, cursor_X_bias, cursor_Y_bias);
                };
            }
        }

        // update from lua
        void update(const std::string &key, const sol::table &value) final {
            if (!value["value"].valid()) return;
            sol:sol::protected_function callback = value["value"];
            if (key == "onMouseDown") {
                onMouseDownCallBack = [callback](const SPW::Entity& e, MouseCode button_code) {
                    callback(e.getUUID().toString(), int(button_code));
                };
            }
            if (key == "onMouseHeld") {
                onMouseHeldCallBack = [callback](const SPW::Entity& e, MouseCode button_code) {
                    callback(e.getUUID().toString(), int(button_code));
                };
            }
            if (key == "onMouseUp") {
                onMouseReleasedCallBack = [callback](const SPW::Entity& e, MouseCode button_code) {
                    callback(e.getUUID().toString(), int(button_code));
                };
            }
            if (key == "onScroll") {
                onMouseScrollCallBack = [callback](const SPW::Entity& e, double scroll_offset) {
                    callback(e.getUUID().toString(), scroll_offset);
                };
            }
            if (key == "onMove") {
                cursorMovementCallBack = [callback](const SPW::Entity& e, double cursor_x, double cursor_y, double cursor_X_bias, double cursor_Y_bias) {
                    callback(e.getUUID().toString(), cursor_x, cursor_y, cursor_X_bias, cursor_Y_bias);
                };
            }
        }
   
    };

}

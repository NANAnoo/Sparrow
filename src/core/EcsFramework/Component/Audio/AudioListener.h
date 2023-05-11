//
// Created by 惠普 on 2023/3/2.
//

#ifndef SPARROW_AUDIOLISTENER_H
#define SPARROW_AUDIOLISTENER_H

#include "../ComponentI.h"
#include "fmod.hpp"
#include "string"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace SPW
{
    class AudioListener : public ComponentI{

    public:
        AudioListener() = default;

        int Listener_id;

        // init from lua
        void initFromLua(const sol::table &value) override {
            // listener_id
            if (value["listener_id"].valid())
                Listener_id = value["listener_id"];
        }

        // no update function

        // getLuaValue
        virtual sol::object getLuaValue(const sol::table &value, const std::string &key) override {
            if (key == "listener_id") {
                return sol::make_object(value.lua_state(), Listener_id);
            }
            return sol::nil;
        }
    };
};
#endif //SPARROW_AUDIOLISTENER_H

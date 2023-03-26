#pragma once
#include <sol/sol.hpp>
#include "Utils/UUID.hpp"

#include "EcsFramework/Component/ComponentI.h"
#include "EcsFramework/Entity/Entity.hpp"

#include <string>
#include <memory.h>

namespace SPW {
    class ComponentWrapper {
        public:
        ComponentWrapper(const std::shared_ptr<Entity>& en) 
            : m_entity(en){}

        template<Component c>
        static ComponentWrapper createCompWrapper(const std::shared_ptr<Entity>& en) 
        {
            ComponentWrapper wrapper(en);
            // check if the entity has the component
            wrapper.comp = en->has<c>() ? en->component<c>() : nullptr;
            return wrapper;
        }
        
        ComponentWrapper(const ComponentWrapper&) = delete;
        ComponentWrapper& operator=(const ComponentWrapper&) = delete;
        ComponentWrapper(ComponentWrapper&&) = default;
        ComponentWrapper& operator=(ComponentWrapper&&) = default;

        template<Component c, typename...Args>
        void setupComponent(const sol::table &value, Args ...args) {
            comp = m_entity->emplace<c>(args...);
            comp->initFromLua(value);
        }

        // update from lua
        void update(const std::string &key, const sol::table &value) {
            if (comp) {
                comp->update(key, value);
            }
        }

        // getLuaValue
        sol::object getCppValue(const sol::table &value, const std::string &key) {
            if (comp) {
                return comp->getLuaValue(value, key);
            }
            return sol::nil;
        }

        // check if component is valid
        bool isValid() {
            return comp != nullptr;
        }

        static void bindLuaTable(sol::table &parent) {
            parent.new_usertype<ComponentWrapper>("ComponentWrapper",
                "update", &ComponentWrapper::update,
                "isValid", &ComponentWrapper::isValid,
                "getCppValue", &ComponentWrapper::getCppValue);
        }

        private:
        std::shared_ptr<Entity> m_entity;
        ComponentI *comp = nullptr;
    };
}
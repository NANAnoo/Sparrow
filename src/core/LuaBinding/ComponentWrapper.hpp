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
        ComponentWrapper(const ComponentWrapper&) = delete;
        ComponentWrapper& operator=(const ComponentWrapper&) = delete;
        ComponentWrapper(ComponentWrapper&&) = default;
        ComponentWrapper& operator=(ComponentWrapper&&) = default;

        void syncValue(const std::string &key, const std::string &value) {
            std::printf("syncValue: %s\n", key.c_str());
        }
        
        static void bindLuaTable(sol::table &parent) {
            parent.new_usertype<ComponentWrapper>("ComponentWrapper",
                "syncValue", &ComponentWrapper::syncValue,
                "update", &ComponentWrapper::update);
        }

        template<Component c, typename...Args>
        void setupComponent(const sol::table &value, Args ...args) {
            comp = m_entity->emplace<c>(std::forward(args)...);
            comp->update(value);
        }

        // update from lua
        void update(const sol::table &value) {
            if (comp) {
                comp->update(value);
            }
        }

        private:
        std::shared_ptr<Entity> m_entity;
        ComponentI *comp;
    };
}
#include "EcsFramework/Entity/Entity.hpp"
#include "Utils/UUID.hpp"
#include <sol/sol.hpp>

#include <string>
#include <memory.h>

namespace SPW {
    class EntityWrapper{
        public:
        EntityWrapper(const std::shared_ptr<Entity> &aEntity)
            : m_entity(aEntity) {

        }

        EntityWrapper(const EntityWrapper&) = delete;
        EntityWrapper& operator=(const EntityWrapper&) = delete;

        EntityWrapper(EntityWrapper &&other) 
            : m_entity(std::move(other.m_entity)) {
        }
        
        EntityWrapper& operator=(EntityWrapper &&other) {
            m_entity = std::move(other.m_entity);
            return *this;
        }
        
        // @lua, getID
        std::string getID() {
            return m_entity->getUUID().toString();
        }

        static void bindLuaTable(sol::table &parent) {
            parent.new_usertype<EntityWrapper>("EntityWrapper",
                "getID", &EntityWrapper::getID);
        }

        private:
        std::shared_ptr<Entity> m_entity;
    };
}
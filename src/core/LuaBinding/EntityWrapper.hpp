#include "EcsFramework/Entity/Entity.hpp"
#include "Utils/UUID.hpp"
#include <sol/sol.hpp>
#include "ComponentWrapper.hpp"

#include <string>
#include <memory.h>

#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/BasicComponent/NameComponent.h"
#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"
#include "EcsFramework/Component/LightComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"

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

        ComponentWrapper createComponent(const std::string &type, const sol::table &value) {
            auto res = ComponentWrapper(m_entity);
            if (type == "Transform") {
                res.setupComponent<TransformComponent>(value);
            }
            return res;
        }

        static void bindLuaTable(sol::table &parent) {
            parent.new_usertype<EntityWrapper>("EntityWrapper",
                "getID", &EntityWrapper::getID,
                "createComponent", &EntityWrapper::createComponent);
        }

        private:
        std::shared_ptr<Entity> m_entity;
    };
}
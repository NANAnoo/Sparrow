#include "EcsFramework/Scene.hpp"
#include "Utils/UUID.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/ModelComponent.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"

#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"


#include "EcsFramework/System/RenderSystem/RenderSystem.h"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"
#include "EcsFramework/System/RenderSystem/RenderSystem.h"
#include "EcsFramework/System/AudioSystem/AudioSystem.h"


#include <string>
#include <unordered_map>


namespace SPW {
    class SceneWrapper{
        public:
        SceneWrapper() = delete;

        SceneWrapper(const SceneWrapper&) = delete;
        SceneWrapper& operator=(const SceneWrapper&) = delete;

        SceneWrapper(SceneWrapper &&other) {
            m_scene = std::move(other.m_scene);
            all_entities = std::move(all_entities);
        }
        
        SceneWrapper& operator=(SceneWrapper &&other) {
            m_scene = std::move(other.m_scene);
            all_entities = std::move(all_entities);
            return *this;
        }

        // TODO: load a scene from path
        SceneWrapper(std::shared_ptr<EventResponderI> parent, std::string path) 
         : m_scene(std::make_shared<Scene>(parent)) {
            std::printf("SceneWrapper init, %p", this);
        }

        // create entity from lua
        std::string createEntity(const std::string &aName) {
            auto newEntity = m_scene->createEntity(aName);
            auto id = newEntity->getUUID().toString();
            all_entities[id] = newEntity;
            return id;
        }

        // remove entity from lua
        void remove(const std::string &uuid) {
            if (all_entities.find(uuid) != all_entities.end()) {
                m_scene->deleteEntity(all_entities[uuid]);
                all_entities.erase(uuid);
            }
        }

        std::shared_ptr<Scene> m_scene;
        std::unordered_map<std::string, std::shared_ptr<Entity>> all_entities;
    };
}
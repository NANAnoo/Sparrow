#pragma once

#include "Utils/UUID.hpp"

#include "Render/DefaultRenderPass.hpp"
#include "EcsFramework/System/NewRenderSystem/SPWRenderSystem.h"
#include "EntityWrapper.hpp"
#include "EcsFramework/Scene.hpp"
#include "LuaBinding/RenderWrapper.hpp"


#include <string>
#include <unordered_map>
#include <sol/sol.hpp>
#include "Render/RenderGraphManager.h"

namespace SPW {
    class SceneWrapper{
        public:
        SceneWrapper() = delete;

        SceneWrapper(const SceneWrapper&) = delete;
        SceneWrapper& operator=(const SceneWrapper&) = delete;

        SceneWrapper(SceneWrapper &&other)  noexcept {
            m_scene = other.m_scene;
            m_renderSystem = other.m_renderSystem;
        }

        // TODO: load a scene from path
        SceneWrapper(const std::shared_ptr<EventResponderI>& parent, const std::string& path)
         : m_scene(std::make_shared<Scene>(parent)) {
        }

        // load from an existing scene
        explicit SceneWrapper(const std::shared_ptr<Scene> &scene)
         : m_scene(scene) {
        }

        // create entity from lua
        [[nodiscard]] EntityWrapper createEntity(const std::string &aName) const {
            auto newEntity = m_scene->createEntity(aName);
            auto id = newEntity->getUUID().toString();
            return {newEntity};
        }

        // create entity wrapper from id
        [[nodiscard]] EntityWrapper getEntityByID(const std::string &id) const {
            auto entity = m_scene->getEntityByID(id);
            return {entity};
        }

        // remove entity from lua
        void remove(const std::string &id) const {
            m_scene->removeEntityByID(id);
        }

        // void initialize
        void initial() const {
            m_scene->initial();
        }

        // before update
        void beforeUpdate() const {
            m_scene->beforeUpdate();
        }

        // update
        void onUpdate(double dt) const {
            m_scene->onUpdate(TimeDuration::second(dt));
        }

        // after update
        void afterUpdate() const {
            m_scene->afterUpdate();
        }

        // stop
        void onStop() const {
            m_scene->onStop();
        }

        void setUpDefaultRenderGraph(
                const std::shared_ptr<SPWRenderSystem> &renderSystem,
                const std::shared_ptr<RenderBackEndI> &backend) {
            m_renderSystem = renderSystem;
            RenderGraphManager::getInstance()->createRenderGraph(backend, kDefferShadingGraph);
            RenderGraphManager::getInstance()->createRenderGraph(backend, kSkyBoxRenderGraph);
            RenderGraphManager::getInstance()->forEachShader([this](const ShaderDesc &shader) {
                m_renderSystem.lock()->addShaderDescriptor(shader);
            });
        }

        [[nodiscard]] unsigned int getGraphID(const std::string &name) const {
            return GET_RENDER_GRAPH(name);
        }

        [[nodiscard]] unsigned int getRenderNodeID(const std::string &name) const {
            return GET_RENDER_NODE(name);
        }

        std::string getShaderID(const std::string &name) {
            return GET_SHADER_DESC(name).uuid.toString();
        }

        AttachmentPort getRenderPort(const std::string &name) {
            return GET_RENDER_OUTPUT(name);
        }

        [[nodiscard]] RenderGraphWrapper createRenderGraph(const std::string &name) const {
            auto backend = m_renderSystem.lock()->renderBackEnd;
            auto graph = RenderGraphManager::getInstance()->createRenderGraph(backend, name);
            return {graph};
        }

        void registerShader(const ShaderDescWrapper &desc) {
            m_renderSystem.lock()->addShaderDescriptor(desc.m_desc);
        }

        static void bindLuaTable(sol::table &parent) {
            parent.new_usertype<SceneWrapper>("SceneWrapper",
                "createEntity", &SPW::SceneWrapper::createEntity,
                "remove", &SceneWrapper::remove,
                "initial", &SceneWrapper::initial,
                "beforeUpdate", &SceneWrapper::beforeUpdate,
                "onUpdate", &SceneWrapper::onUpdate,
                "afterUpdate", &SceneWrapper::afterUpdate,
                "onStop", &SceneWrapper::onStop,
                "getEntityByID", &SceneWrapper::getEntityByID,
                "getGraphID", &SceneWrapper::getGraphID,
                "getRenderNodeID", &SceneWrapper::getRenderNodeID,
                "getShaderID", &SceneWrapper::getShaderID,
                "getRenderPort", &SceneWrapper::getRenderPort,
                "createRenderGraph", &SceneWrapper::createRenderGraph,
                "registerShader", &SceneWrapper::registerShader
            );
        }

        std::shared_ptr<Scene> m_scene;
        std::weak_ptr<SPWRenderSystem> m_renderSystem;
    };
}
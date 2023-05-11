#pragma once

#include "Utils/UUID.hpp"

#include "EcsFramework/System/NewRenderSystem/DefaultRenderPass.hpp"
#include "EcsFramework/System/NewRenderSystem/SPWRenderSystem.h"
#include "EntityWrapper.hpp"
#include "EcsFramework/Scene.hpp"
#include "LuaBinding/RenderWrapper.hpp"


#include <string>
#include <unordered_map>
#include <sol/sol.hpp>


namespace SPW {
    class SceneWrapper{
        public:
        SceneWrapper() = delete;

        SceneWrapper(const SceneWrapper&) = delete;
        SceneWrapper& operator=(const SceneWrapper&) = delete;

        SceneWrapper(SceneWrapper &&other) {
            m_scene = other.m_scene;
            m_renderSystem = other.m_renderSystem;

            // graph id
            pbrShadowGraphID = other.pbrShadowGraphID;
            skyboxGraphID = other.skyboxGraphID;

            // node id
            pointShadowNodeID = other.pointShadowNodeID;
            dirShadowNodeID = other.dirShadowNodeID;
            pbrShadowLightingNodeID = other.pbrShadowLightingNodeID;
            skyboxNodeID = other.skyboxNodeID;

            // shader id
            pbrShadowShaderID = other.pbrShadowShaderID;
            pbrShadowAniShaderID = other.pbrShadowAniShaderID;
            pointShadowShaderID =  other.pointShadowShaderID;
            dirShadowShaderID = other.dirShadowShaderID;
            pointShadowAniShaderID = other.pointShadowAniShaderID;
            dirShadowAniShaderID = other.dirShadowAniShaderID;
            skyboxShaderID = other.skyboxShaderID;
        }
        
        SceneWrapper& operator=(SceneWrapper &&other) {
            *this = std::move(other);
            return *this;
        }

        // TODO: load a scene from path
        SceneWrapper(std::shared_ptr<EventResponderI> parent, std::string path) 
         : m_scene(std::make_shared<Scene>(parent)) {
        }

        // load from an existing scene
        SceneWrapper(const std::shared_ptr<Scene> &scene) 
         : m_scene(scene) {
        }

        // create entity from lua
        EntityWrapper createEntity(const std::string &aName) {
            auto newEntity = m_scene->createEntity(aName);
            auto id = newEntity->getUUID().toString();
            return EntityWrapper(newEntity);
        }

        // create entity wrapper from id
        EntityWrapper getEntityByID(const std::string &id) {
            auto entity = m_scene->getEntityByID(id);
            return EntityWrapper(entity);
        }

        // remove entity from lua
        void remove(const std::string &id) {
            m_scene->removeEntityByID(id);
        }

        // void initialize
        void initial() {
            m_scene->initial();
        }

        // before update
        void beforeUpdate() {
            m_scene->beforeUpdate();
        }

        // update
        void onUpdate(double dt) {
            m_scene->onUpdate(TimeDuration::second(dt));
        }

        // after update
        void afterUpdate() {
            m_scene->afterUpdate();
        }

        // stop
        void onStop() {
            m_scene->onStop();
        }

        void setUpDefaultRenderGraph(std::shared_ptr<SPWRenderSystem> &rendersystem) {
            m_renderSystem = rendersystem;
            // ------ create main render graph ----------------
            auto pbr_with_PDshadow = rendersystem->createRenderGraph();

            auto p_shadowmap_node = pbr_with_PDshadow->createRenderNode<SPW::ModelRepeatPassNode>(SPW::CubeMapType, SPW::RepeatForPLights, 10);
            p_shadowmap_node->width = 256;
            p_shadowmap_node->height = 256;
            p_shadowmap_node->clearType = SPW::ClearDepth;

            auto d_shadowmap_node = pbr_with_PDshadow->createRenderNode<SPW::ModelRepeatPassNode>(SPW::ColorType, SPW::RepeatForDLights, 10);
            d_shadowmap_node->width = 4096;
            d_shadowmap_node->height = 4096;
            d_shadowmap_node->clearType = SPW::ClearDepth;

            auto pbr_shadow_lighting_node = pbr_with_PDshadow->createRenderNode<SPW::ModelToScreenNode>();

            auto p_shadowmap_output = p_shadowmap_node->addAttachment(SPW::Depth);
            auto d_shadowmap_output = d_shadowmap_node->addAttachment(SPW::Depth);

            pbr_shadow_lighting_node->bindInputPort(p_shadowmap_output);
            pbr_shadow_lighting_node->bindInputPort(d_shadowmap_output);
            
            auto pbr_shadow_lighting_output = pbr_shadow_lighting_node->addScreenAttachment(SPW::ScreenColorType);
            // ------ create main render graph ----------------

            // ------ create post processing graph --------------
            SPW::AttachmentPort screen_color_port = {SPW::SCREEN_PORT, SPW::ScreenColorType};
            rendersystem->presentNode = rendersystem->postProcessGraph->createRenderNode<SPW::PresentNode>(FXAA_desc(screen_color_port));
            rendersystem->presentNode->bindInputPort(screen_color_port);
            rendersystem->presentNode->depthTest = false;
            // ------ create post processing graph --------------

            // --------------- create shader ---------------
            SPW::ShaderHandle pbr_light_shadow({
                                         "pbr_light_shadow",
                                         "./resources/shaders/simpleVs.vert",
                                         "./resources/shaders/pbrShadow.frag"
                                     });
            SPW::ShaderHandle pbr_ani_light_shadow({
                                         "pbr_light_shadow",
                                         "./resources/shaders/ani_model.vert",
                                         "./resources/shaders/pbrShadow.frag"
                                     });

            auto p_shadow_desc = SPW::P_shadowmap_desc();
            auto d_shadow_desc = SPW::D_shadowmap_desc();
            auto p_ani_shadow_desc = SPW::P_ani_shadowmap_desc();
            auto d_ani_shadow_desc = SPW::D_ani_shadowmap_desc();

            auto pbr_ani_light_shadow_desc = PBR_ani_shadow_desc(p_shadowmap_output, d_shadowmap_output, pbr_ani_light_shadow);
            auto pbr_light_shadow_desc = PBR_light_with_shadow_desc(p_shadowmap_output, d_shadowmap_output, pbr_light_shadow);
            
            auto skybox_desc = SPW::SkyBoxShader_desc();
            auto ui_desc = SPW::UIShader();
            rendersystem->addShaderDesciptor(pbr_light_shadow_desc);
            rendersystem->addShaderDesciptor(p_shadow_desc);
            rendersystem->addShaderDesciptor(d_shadow_desc);
            rendersystem->addShaderDesciptor(skybox_desc);
            rendersystem->addShaderDesciptor(p_ani_shadow_desc);
            rendersystem->addShaderDesciptor(d_ani_shadow_desc);
            rendersystem->addShaderDesciptor(pbr_ani_light_shadow_desc);
            rendersystem->addShaderDesciptor(*ui_desc);

            pbrShadowGraphID = pbr_with_PDshadow->graph_id;
            skyboxGraphID = rendersystem->skyBoxGraph->graph_id;

            pointShadowNodeID = p_shadowmap_node->pass_id;
            dirShadowNodeID = d_shadowmap_node->pass_id;
            pbrShadowLightingNodeID = pbr_shadow_lighting_node->pass_id;
            skyboxNodeID = rendersystem->skyBoxNode->pass_id;

            pbrShadowShaderID = pbr_light_shadow_desc.uuid;
            pbrShadowAniShaderID = pbr_ani_light_shadow_desc.uuid;
            pointShadowShaderID = p_shadow_desc.uuid;
            dirShadowShaderID = d_shadow_desc.uuid;
            pointShadowAniShaderID = p_ani_shadow_desc.uuid;
            dirShadowAniShaderID = d_ani_shadow_desc.uuid;
            skyboxShaderID = skybox_desc.uuid;
            uiShaderID = ui_desc->uuid;
        }

        unsigned int getGraphID(const std::string &name) {
            if (name == "PBRShadowGraph") {
                return pbrShadowGraphID;
            } else if (name == "SkyBoxGraph") {
                return skyboxGraphID;
            }
            return 0;
        }

        unsigned int getRenderNodeID(const std::string &name) {
            if (name == "PointShadowNode") {
                return pointShadowNodeID;
            } else if (name == "DirectionalShadowNode") {
                return dirShadowNodeID;
            } else if (name == "PBRShadowLightingNode") {
                return pbrShadowLightingNodeID;
            } else if (name == "SkyBoxNode") {
                return skyboxNodeID;
            }
            return 0;
        }

        std::string getShaderID(const std::string &name) {
            if (name == "PBRShadowShader") {
                return pbrShadowShaderID.toString();
            } else if (name == "PBRShadowAniShader") {
                return pbrShadowAniShaderID.toString();
            } else if (name == "PointShadowShader") {
                return pointShadowShaderID.toString();
            } else if (name == "DirectionalShadowShader") {
                return dirShadowShaderID.toString();
            } else if (name == "PointShadowAniShader") {
                return pointShadowAniShaderID.toString();
            } else if (name == "DirectionalShadowAniShader") {
                return dirShadowAniShaderID.toString();
            } else if (name == "SkyBoxShader") {
                return skyboxShaderID.toString();
            }
            return "";
        }

        RenderGraphWrapper createRenderGraph() {
            auto graph = m_renderSystem.lock()->createRenderGraph();
            return RenderGraphWrapper(graph);
        }

        void registerShader(const ShaderDescWrapper &desc) {
            m_renderSystem.lock()->addShaderDesciptor(desc.m_desc);
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
                "createRenderGraph", &SceneWrapper::createRenderGraph,
                "registerShader", &SceneWrapper::registerShader
            );
        }

        std::shared_ptr<Scene> m_scene;
        std::weak_ptr<SPWRenderSystem> m_renderSystem;

        // default render graph
        unsigned int pbrShadowGraphID = 0;
        unsigned int skyboxGraphID = 0;

        // default renderNodes
        unsigned int pointShadowNodeID = 0;
        unsigned int dirShadowNodeID = 0;
        unsigned int pbrShadowLightingNodeID = 0;
        unsigned int skyboxNodeID = 0;

        // default shader id
        UUID pbrShadowShaderID = UUID::noneID();
        UUID pbrShadowAniShaderID = UUID::noneID();
        UUID pointShadowShaderID = UUID::noneID();
        UUID dirShadowShaderID = UUID::noneID();
        UUID pointShadowAniShaderID = UUID::noneID();
        UUID dirShadowAniShaderID = UUID::noneID();
        UUID skyboxShaderID = UUID::noneID();
        UUID uiShaderID = UUID::noneID();
    };
}
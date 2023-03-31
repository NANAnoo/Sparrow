//
// Created by ZhangHao on 2023/2/19.
//
#pragma once

#include "EcsFramework/Entity/Entity.hpp"
#include "Render/RenderBackEndI.h"
#include "Render/PipeLine.hpp"

#include "EcsFramework/System/SystemI.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"

#include "RenderSubPass.hpp"

namespace SPW {

    class FlexibleRenderSystem : public SystemI, public WindowEventResponder {
    public:
        explicit FlexibleRenderSystem(std::shared_ptr<Scene> &scene, std::shared_ptr<RenderBackEndI> backEnd, int w, int h) :
            SystemI(scene), renderBackEnd(std::move(backEnd)),
            WindowEventResponder(std::dynamic_pointer_cast<EventResponderI>(scene)) {
                width = w;
                height = h;
            }
        void setupRenderBackEnd(const std::shared_ptr<RenderBackEndI> &backEnd) {
            renderBackEnd = backEnd;
        };

        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;

        // add render passes
        void addRenderPass(RenderPassConfig pass) {
            // precheck
            pass.id = RenderPasses.size();
            if (!pass.model_subpass.empty()) {
                unsigned int subpass = pass.model_subpass[0];
                // fisrt model subpass should require no previous_attachment_inputs
                assert(ModelPasses[subpass].config.previous_attachment_inputs.size() > 0);
            }
            RenderPasses.push_back(pass);
        }

        void setUIPass(const RenderPassConfig &pass) {
            uiPass = pass;
        }

        void addModelSubPass(ModelSubPassConfig pass) {
            pass.ID = ModelPasses.size();
            ModelPasses.push_back(pass);
        }

        void addImageSubPass(ImageSubPassConfig pass) {
            pass.ID = ImagePasses.size();
            ImagePasses.push_back(pass);
        }
        // events
        const char *getName() override {return "SPW_RENDER_SYSTEM";}
        bool onFrameResize(int w, int h) override;
    private:
        using ShaderModelMap = std::unordered_map<ShaderHandle, std::vector<SPW::Entity>, ShaderHash, ShaderEqual>;
        void renderModelSubPasses(const glm::mat4x4 &V, const glm::mat4x4 &P, const std::vector<Entity> &models, const std::vector<unsigned int> &subPassIDs);
        void renderModelSubPass(const glm::mat4x4 &V, const glm::mat4x4 &P, const ShaderModelMap &models, unsigned int subPassID);
        void renderModelSubPassOnDlight(const glm::mat4x4 &V, const glm::mat4x4 &P, const ShaderModelMap &models, unsigned int subPassID, unsigned int dlightID);
        void renderModelSubPassOnPlight(const glm::mat4x4 &V, const glm::mat4x4 &P, ShaderModelMap &models, unsigned int subPassID, unsigned int plightID);
        
        void renderImageSubPasses(unsigned int dependModelPass, const std::vector<unsigned int> &subPassIDs);
        void findAllLights();
        std::shared_ptr<FrameBuffer> screenBuffer;
        std::shared_ptr<RenderBackEndI> renderBackEnd;

        std::vector<RenderPassConfig> RenderPasses;
        std::vector<SPWModelSubPass> ModelPasses;
        std::vector<SPWImageSubPass> ImagePasses;

        std::vector<PLight> pLights;
        std::vector<DLight> dLights;

        RenderPassConfig uiPass;

        unsigned int width;
        unsigned int height;
    };
}

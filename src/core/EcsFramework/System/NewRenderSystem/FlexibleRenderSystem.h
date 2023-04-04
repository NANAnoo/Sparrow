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
        unsigned int addRenderPass(RenderPassConfig pass) {
            // precheck
            pass.id = RenderPasses.size() + 1;
            if (!pass.model_subpass.empty()) {
                unsigned int subpass = pass.model_subpass[0] - 1;
                // fisrt model subpass should require no previous_attachment_inputs
                assert(ModelPasses[subpass].config.previous_attachment_inputs.size() == 0);
                unsigned int last_subpass = pass.model_subpass.back();
                for (unsigned int id : pass.model_subpass) {
                    assert(id - 1 < ModelPasses.size());
                    bool has_present = false;
                    if (ModelPasses[id - 1].config.screenOutputType == PresentType) {
                        // if this subpass output to screen, then it should be the last subpass
                        assert(id == last_subpass);
                        has_present = true;
                    }
                    if (has_present) {
                        // if this subpass output to screen, there should be no image subpass
                        assert(pass.image_subpass.empty());
                    }
                }
            } else if (!pass.image_subpass.empty()){
                // if model subpass is empty, then the first image subpass should require no previous_attachment_inputs
                unsigned int subpass = pass.image_subpass[0] - 1;
                assert(ImagePasses[subpass].config.previous_attachment_inputs.size() == 0);
                unsigned int last_subpass = pass.image_subpass.back();
                for (unsigned int id : pass.image_subpass) {
                    assert(id - 1 < ImagePasses.size());
                    bool has_present = false;
                    if (ImagePasses[id - 1].config.screenOutputType == PresentType) {
                        // if this subpass output to screen, then it should be the last subpass
                        assert(id == last_subpass);
                        has_present = true;
                    }
                }
            }
            RenderPasses.push_back(pass);
            return pass.id;
        }

        void setUIPass(const RenderPassConfig &pass) {
            uiPass = pass;
            uiPass.id = 777;
        }

        void setPostProcessPass(const RenderPassConfig &pass) {
            postProcessPass = pass;
            postProcessPass.id = 888;
        }

        unsigned int addModelSubPass(ModelSubPassConfig pass) {
            pass.ID = ModelPasses.size() + 1;
            ModelPasses.push_back(pass);
            return pass.ID;
        }

        unsigned int addImageSubPass(ImageSubPassConfig pass) {
            pass.ID = ImagePasses.size() + 1;
            ImagePasses.push_back(pass);
            return pass.ID;
        }
        // events
        const char *getName() override {return "SPW_RENDER_SYSTEM";}
        bool onFrameResize(int w, int h) override;
    private:
        using ShaderModelMap = std::unordered_map<ShaderHandle, std::vector<SPW::Entity>, ShaderHash>;
        void renderModelSubPasses(
            const glm::vec4 &camPos, const glm::vec3 &cam_center,
            const glm::mat4x4 &V, const glm::mat4x4 &P, 
            const std::vector<Entity> &models, const std::vector<unsigned int> &subPassIDs);
        void renderModelSubPass(
            const glm::vec4 &camPos, const glm::vec3 &cam_center,
            const glm::mat4x4 &V, const glm::mat4x4 &P, 
            const ShaderModelMap &models, unsigned int subPassID, unsigned int dependPassID,
            const RepeatType repeatType, unsigned int repeatID);
        
        void renderImageSubPasses(const glm::vec4 &camPos, const glm::vec3 &cam_center, unsigned int dependModelPass, const std::vector<unsigned int> &subPassIDs);
        void findAllLights();

        std::shared_ptr<FrameBuffer> screenBuffer;
        std::shared_ptr<AttachmentTexture> screenTexture;
        std::shared_ptr<AttachmentTexture> screenDepthTexture;
        
        std::shared_ptr<RenderBackEndI> renderBackEnd;

        std::vector<RenderPassConfig> RenderPasses;
        std::vector<SPWModelSubPass> ModelPasses;
        std::vector<SPWImageSubPass> ImagePasses;

        std::vector<PLight> pLights;
        std::vector<DLight> dLights;

        RenderPassConfig postProcessPass;
        RenderPassConfig uiPass;

        unsigned int width;
        unsigned int height;
    };
}

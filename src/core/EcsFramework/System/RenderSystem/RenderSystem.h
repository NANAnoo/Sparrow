//
// Created by ZhangHao on 2023/2/19.
//

#ifndef SPARROW_RENDERSYSTEM_H
#define SPARROW_RENDERSYSTEM_H

#include "Render/RenderBackEndI.h"
#include "Render/RenderPass.hpp"
#include "EcsFramework/System/SystemI.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"


namespace SPW {
    using RenderCamera = std::tuple<IDComponent *, CameraComponent *, TransformComponent *>;
    class RenderSystem : public SystemI, public WindowEventResponder {
    public:
        explicit RenderSystem(std::shared_ptr<Scene> &scene, std::shared_ptr<RenderBackEndI> backEnd, int w, int h) :
            SystemI(scene), renderBackEnd(std::move(backEnd)),
            WindowEventResponder(std::dynamic_pointer_cast<EventResponderI>(scene)) {
                frameBuffer = renderBackEnd->creatSenceFrameBuffer();
                frameBuffer->genFrameBuffer();
                frameBuffer->bind();
                frameBuffer->AttachColorTexture(w,h,0);
                frameBuffer->AttachDepthRenderBuffer(w,h);
                frameBuffer->CheckFramebufferStatus();
                frameBuffer->unbind();
                width = w;
                height = h;

                //depthBuffer = renderBackEnd->creatSenceFrameBuffer();
                //depthBuffer->genFrameBuffer();
                //depthBuffer->AttachDepthTexture();
                //depthBuffer->unbind();
        }
        void setupRenderBackEnd(const std::shared_ptr<RenderBackEndI> &backEnd) {
            renderBackEnd = backEnd;
        };
        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;

        // events
        const char *getName() override {return "SPW_RENDER_SYSTEM";}
        bool onFrameResize(int w, int h) override;
        RenderPass postProcessPass;

    private:
        void renderModelsWithCamera(const RenderCamera &camera,glm::mat4& View,glm::mat4& Pro);
        void findAllLights(std::vector<PLight> &Plights, std::vector<DLight> &Dlights);
        std::shared_ptr<RenderBackEndI> renderBackEnd;
        std::shared_ptr<SPW::FrameBuffer> frameBuffer;
        std::shared_ptr<SPW::FrameBuffer> depthBuffer;
        int width;
        int height;
    };
}

#endif //SPARROW_RENDERSYSTEM_H

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
        explicit RenderSystem(std::shared_ptr<Scene> &scene, std::shared_ptr<RenderBackEndI> backEnd) :
            SystemI(scene), renderBackEnd(std::move(backEnd)),
            WindowEventResponder(std::dynamic_pointer_cast<EventResponderI>(scene)) {

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

    private:
        void renderModelsWithCamera(const RenderCamera &camera);
        std::shared_ptr<RenderBackEndI> renderBackEnd;
        RenderPass postProcessPass;
    };
}

#endif //SPARROW_RENDERSYSTEM_H

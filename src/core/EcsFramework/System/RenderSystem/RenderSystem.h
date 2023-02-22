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



namespace SPW {
    using RenderCamera = std::tuple<IDComponent *, CameraComponent *, TransformComponent *>;
    class RenderSystem : public SystemI{
    public:
        explicit RenderSystem(std::shared_ptr<Scene> &scene, std::shared_ptr<RenderBackEndI> backEnd) :
            SystemI(scene), renderBackEnd(std::move(backEnd)) {
        }
        void setupRenderBackEnd(const std::shared_ptr<RenderBackEndI> &backEnd) {
            renderBackEnd = backEnd;
        };
        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;
    private:
        void renderModelsWithCamera(const RenderCamera &camera);
        std::shared_ptr<RenderBackEndI> renderBackEnd;
        RenderPass postProcessPass;
    };
}

#endif //SPARROW_RENDERSYSTEM_H

//
// Created by ZhangHao on 2023/2/19.
//

#ifndef SPARROW_RENDERSYSTEM_H
#define SPARROW_RENDERSYSTEM_H

#include "Render/RenderAPII.h"
#include "Render/RenderPass.hpp"
#include "EcsFramework/System/SystemI.h"

namespace SPW {
    class RenderSystem : public SystemI{
    public:
        void setupRenderBackEnd(const std::shared_ptr<RenderAPII> &backEnd) {
            api = backEnd;
        };
        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;
    private:
        std::shared_ptr<RenderAPII> api;
        RenderPass shadowPass;
        RenderPass modelPass;
        RenderCommand postProcess;
    };
}

#endif //SPARROW_RENDERSYSTEM_H

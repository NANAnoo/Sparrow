//
// Created by ZhangHao on 2023/2/19.
//

#ifndef SPARROW_RENDERPASS_HPP
#define SPARROW_RENDERPASS_HPP


#include "RenderCommandsQueue.hpp"

namespace SPW{
    class RenderPass {
    public:
        RenderPass() {
            queue = std::make_shared<RenderCommandsQueue>();
        }
        RenderPass(RenderPass &&other) noexcept {
            queue = other.queue;
        }
        RenderPass(const RenderPass &other) {
            queue = other.queue;
        }
        void pushCommand(RenderCommand &&command) {
            queue->pushCommand(std::move(command));
        }
        void executeWithAPI(std::shared_ptr<RenderBackEndI> &backEnd) {
            queue->executeWithAPI(backEnd);
        }
    private:
        std::shared_ptr<RenderCommandsQueue> queue;
    };
}


#endif //SPARROW_RENDERPASS_HPP

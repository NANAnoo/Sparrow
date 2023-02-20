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
        void pushCommand(const RenderCommand &command) {
            queue->pushCommand(command);
        }
        void executeWithAPI(RenderBackEndI &API) {
            queue->executeWithAPI(API);
        }
    private:
        std::shared_ptr<RenderCommandsQueue> queue;
    };
}


#endif //SPARROW_RENDERPASS_HPP

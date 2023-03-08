//
// Created by ZhangHao on 2023/2/19.
//

#ifndef SPARROW_RENDERPASS_HPP
#define SPARROW_RENDERPASS_HPP


#include "RenderCommandsQueue.hpp"

namespace SPW{
    template <class API>
    class RenderPass {
    public:
        RenderPass() {
            queue = std::make_shared<RenderCommandsQueue<API>>();
        }
        RenderPass(RenderPass &&other) noexcept {
            queue = other.queue;
        }
        RenderPass(const RenderPass &other) {
            queue = other.queue;
        }
        void pushCommand(RenderCommand<API> &&command) {
            queue->pushCommand(std::move(command));
        }
        void executeWithAPI(std::shared_ptr<API> &api) {
            queue->executeWithAPI(api);
        }
    private:
        std::shared_ptr<RenderCommandsQueue<API>> queue;
    };
}


#endif //SPARROW_RENDERPASS_HPP

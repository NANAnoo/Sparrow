//
// Created by ZhangHao on 2023/3/8.
//

#ifndef SPARROW_UNIFORMBUFFER_H
#define SPARROW_UNIFORMBUFFER_H

#include <memory>
#include <string>
#include "RenderCommandsQueue.hpp"

namespace SPW {
    class UBO_I {
    public:
        UBO_I() = default;
        virtual void bindSubData(void *data, unsigned int offset, unsigned int size) = 0;
        virtual bool isReady() = 0;
    };
    class UniformBuffer {
    public:
        UniformBuffer() = delete;
        explicit UniformBuffer(std::string aName, unsigned int size, unsigned int slot) :
            name(std::move(aName)),bufferSize (size), slot(slot) {

        }
        void updateSubData(void *data, unsigned int offset, unsigned int size) {
            if (ubo != nullptr && ubo->isReady()) {
                ubo->bindSubData(data, offset, size);
            } else {
                commands.pushCommand(RenderCommand(&UBO_I::bindSubData, data, offset, size));
            }
        }

        void flush() {
            if (ubo != nullptr) {
                commands.executeWithAPI(ubo);
            }
        }

        RenderCommandsQueue<UBO_I> commands;
        std::shared_ptr<UBO_I> ubo;
        std::string name;
        unsigned int slot;
        unsigned int bufferSize;
    };

}

#endif //SPARROW_UNIFORMBUFFER_H

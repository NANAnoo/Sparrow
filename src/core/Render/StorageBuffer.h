//
// Created by ZhangHao on 2023/3/8.
//

#ifndef SPARROW_STORAGEBUFFER_H
#define SPARROW_STORAGEBUFFER_H

#include <memory>
#include <string>
#include "RenderCommandsQueue.hpp"

namespace SPW {
    class SSBO_I {
    public:
        SSBO_I() = default;
        virtual void bindSubData(void *data, unsigned int offset, unsigned int size) = 0;
        virtual bool isReady() = 0;
    };
    class StorageBuffer {
    public:
        StorageBuffer() = delete;
        explicit StorageBuffer(std::string aName, unsigned int size, unsigned int slot) :
            name(std::move(aName)),bufferSize (size), slot(slot) {

        }
        void updateSubData(void *data, unsigned int offset, unsigned int size) {
            if (ssbo != nullptr && ssbo->isReady()) {
                ssbo->bindSubData(data, offset, size);
            } else {
                commands.pushCommand(RenderCommand(&SSBO_I::bindSubData, data, offset, size));
            }
        }

        void flush() {
            if (ssbo != nullptr) {
                commands.executeWithAPI(ssbo);
            }
        }

        RenderCommandsQueue<SSBO_I> commands;
        std::shared_ptr<SSBO_I> ssbo;
        std::string name;
        unsigned int slot;
        unsigned int bufferSize;
    };

}

#endif //SPARROW_STORAGEBUFFER_H

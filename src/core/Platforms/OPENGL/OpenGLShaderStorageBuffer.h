//
// Created by ZhangHao on 2023/3/8.
//

#ifndef SPARROW_OPENGLSHADERSTORAGEBUFFER_H
#define SPARROW_OPENGLSHADERSTORAGEBUFFER_H

#include "Render/StorageBuffer.h"

namespace SPW {
    class OpenGLShader;
    class OpenGLBackEnd;
    class OpenGLSSBO : public SSBO_I{
    public:
        OpenGLSSBO() = default;
        void alloc(unsigned int size);
        void bindSubData(void *data, unsigned int offset, unsigned int size) override;
        bool isReady() override {return ready;};
    private:
        unsigned int SSBO_id = 0;
        bool ready = false;
        friend OpenGLShader;
        friend OpenGLBackEnd;
    };
}

#endif //SPARROW_OPENGLSHADERSTORAGEBUFFER_H

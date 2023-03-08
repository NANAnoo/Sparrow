//
// Created by ZhangHao on 2023/3/8.
//

#ifndef SPARROW_OPENGLUNIFORMBUFFER_H
#define SPARROW_OPENGLUNIFORMBUFFER_H

#include "Render/UniformBuffer.h"

namespace SPW {
    class OpenGLShader;
    class OpenGLBackEnd;
    class OpenGLUBO : public UBO_I{
    public:
        OpenGLUBO() = default;
        void alloc(unsigned int size);
        void bindSubData(void *data, unsigned int offset, unsigned int size) override;
        bool isReady() override {return ready;};
    private:
        unsigned int UBO_id = 0;
        bool ready = false;
        friend OpenGLShader;
        friend OpenGLBackEnd;
    };
}

#endif //SPARROW_OPENGLUNIFORMBUFFER_H

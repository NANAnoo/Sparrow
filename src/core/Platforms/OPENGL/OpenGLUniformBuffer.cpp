//
// Created by ZhangHao on 2023/3/8.
//

#include "OpenGLUniformBuffer.h"
#include "glad/glad.h"


namespace SPW {
    // create an empty buffer
    void OpenGLUBO::alloc(unsigned int size) {
        glGenBuffers(1, &UBO_id);
        glBindBuffer(GL_UNIFORM_BUFFER, UBO_id);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    // bind sub buffer data
    void OpenGLUBO::bindSubData(void *data, unsigned int offset, unsigned int size) {
        // bind buffer
        glBindBuffer(GL_UNIFORM_BUFFER, UBO_id);
        // bind data
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        // unbind buffer
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}
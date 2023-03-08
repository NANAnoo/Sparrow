//
// Created by ZhangHao on 2023/3/8.
//

#include "OpenGLShaderStorageBuffer.h"
#include "glad/glad.h"


namespace SPW {
    // create an empty buffer
    void OpenGLSSBO::alloc(unsigned int size) {
        glGenBuffers(1, &SSBO_id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // bind sub buffer data
    void OpenGLSSBO::bindSubData(void *data, unsigned int offset, unsigned int size) {
        // bind buffer
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_id);
        // bind data
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
        // unbind buffer
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}
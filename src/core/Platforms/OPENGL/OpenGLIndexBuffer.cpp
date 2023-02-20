//
// Created by sgsdi on 2023/2/17.
//

#include "OpenGLIndexBuffer.h"
#include "assert.h"

namespace SPW
{

    void OpenGLIndexBuffer::Bind()
    {
        if(m_indices.size()<1) return assert("No data in indexBuffer");
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    }
    void OpenGLIndexBuffer::UnBind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }
}

//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_OPENGLINDEXBUFFER_H
#define SPARROW_OPENGLINDEXBUFFER_H
#include "Render/IndexBuffer.h"
#include "IO/AttriVertex.h"
#include <glad/glad.h>

namespace SPW
{
    class OpenGLIndexBuffer:public IndexBuffer
    {
    public:
        void Bind() final;
        void UnBind() final;

        OpenGLIndexBuffer(std::vector<unsigned int>& indices):m_indices(indices)
        {
            glGenBuffers(1,&EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
        }

    private:
        std::vector<unsigned int> m_indices;
        unsigned int EBO;
    };


}


#endif //SPARROW_OPENGLINDEXBUFFER_H

//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_OPENGLVERTEXBUFFER_H
#define SPARROW_OPENGLVERTEXBUFFER_H
#include <Render/VertexBuffer.h>
#include <glad/glad.h>
#include "Model/Vertex.h"

namespace SPW
{
    class OpenGLVertexBuffer:public VertexBufferI
    {
    public:
        OpenGLVertexBuffer() = default;
        virtual void GenVertexBuffer() final;
        virtual void GenVertexArray() final;
        virtual void VertexBufferData(std::vector<Vertex>& vertices) final;
        virtual void Bind(){glBindVertexArray(VAO); }
        virtual void UnBind() { glBindVertexArray(0);}

    private:
        unsigned int VAO,VBO;
    };
}




#endif //SPARROW_OPENGLVERTEXBUFFER_H

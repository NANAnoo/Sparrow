//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_VERTEXBUFFER_H
#define SPARROW_VERTEXBUFFER_H
#include <IO/AttriVertex.h>
#include <memory>
#include <vector>
namespace SPW
{
    class VertexBufferI
    {
    public:
        VertexBufferI()= default;
        virtual void GenVertexBuffer()=0;
        virtual void GenVertexArray()=0;
        virtual void VertexBufferData(std::vector<AttribVertex> vertices)=0;
        virtual void Bind()=0;
        virtual void UnBind()=0;

    };

}



#endif //SPARROW_VERTEXBUFFER_H

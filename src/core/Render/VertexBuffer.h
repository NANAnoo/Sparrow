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
        static std::shared_ptr<VertexBufferI> CreateVertexBuffer();
        virtual void GenVertexBuffer();
        virtual void GenVertexArray();
        virtual void VertexBufferData(std::vector<AttribVertex> vertices);
        virtual void Bind();
        virtual void UnBind();

    };

}



#endif //SPARROW_VERTEXBUFFER_H

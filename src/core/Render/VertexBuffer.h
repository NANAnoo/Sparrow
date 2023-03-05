//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_VERTEXBUFFER_H
#define SPARROW_VERTEXBUFFER_H
#include <memory>
#include <vector>
namespace SPW
{
    struct Vertex;
    class VertexBufferI
    {
    public:
        VertexBufferI()= default;
        virtual void GenVertexBuffer()=0;
        virtual void GenVertexArray()=0;
        virtual void VertexBufferData(std::vector<Vertex>& vertices)=0;
        virtual void Bind()=0;
        virtual void UnBind()=0;

    };

}



#endif //SPARROW_VERTEXBUFFER_H

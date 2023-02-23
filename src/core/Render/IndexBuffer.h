//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_INDEXBUFFER_H
#define SPARROW_INDEXBUFFER_H

#include <vector>
#include <memory>

namespace SPW
{
    class IndexBuffer
    {
    public:
        IndexBuffer() = default;
        IndexBuffer(std::vector<unsigned int>& indices):size(indices.size()){ }
        virtual void Bind()=0;
        virtual void UnBind()=0;
        ~IndexBuffer() = default;
    public:
        unsigned int size;

    };
}



#endif //SPARROW_INDEXBUFFER_H

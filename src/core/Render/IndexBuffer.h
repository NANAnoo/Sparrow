//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_INDEXBUFFER_H
#define SPARROW_INDEXBUFFER_H

#include <vector>
#include <IO/AttriVertex.h>
#include <memory>
#include <Render/RenderAPII.h>

namespace SPW
{
    class IndexBuffer
    {
    public:
        virtual void Bind()=0;
        virtual void UnBind()=0;
        //virtual void Bind();
        static std::shared_ptr<IndexBuffer> GenIndexBuffer(std::vector<unsigned int>& indices);

        ~IndexBuffer() = default;

    };
}



#endif //SPARROW_INDEXBUFFER_H

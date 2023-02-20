//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_OPENGLBACKEND_H
#define SPARROW_OPENGLBACKEND_H
#include "Render/RenderBackEndI.h"
#include <glad/glad.h>
#include "OpenGLIndexBuffer.h"
#include "OpenGLVertexBuffer.h"
namespace SPW
{
    class OpenGLBackEnd: public RenderBackEndI
    {
        virtual void Init();
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        virtual void SetClearColor(const glm::vec4& color);
        virtual void Clear();
        virtual void DrawElement(const std::shared_ptr<VertexBufferI>& vertexBuffer ,const std::shared_ptr<IndexBuffer>& indexBuffer);
        //depth
        virtual void DepthTest(bool enable);
        virtual void DepthMask(bool maskFlag);
        virtual void DepthFunc(DepthComp comp);
        //cull
        virtual void Cull(int32_t Bit);
        virtual void CullFrontOrBack(bool bFront);

        //for draw
        virtual std::shared_ptr<IndexBuffer> createIndexBuffer(std::vector<unsigned int> indices) final
        {
            return std::make_shared<OpenGLIndexBuffer>(indices);
        }

        virtual std::shared_ptr<VertexBufferI> createVertexBuffer() final
        {
            std::shared_ptr<VertexBufferI> VB = std::make_shared<OpenGLVertexBuffer>();
            VB->GenVertexArray();
            VB->GenVertexBuffer();
            return VB;
        }
    };
}



#endif //SPARROW_OPENGLBACKEND_H

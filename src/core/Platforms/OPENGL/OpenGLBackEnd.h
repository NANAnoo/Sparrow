//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_OPENGLBACKEND_H
#define SPARROW_OPENGLBACKEND_H
#include "Render/RenderBackEndI.h"
#include <glad/glad.h>
#include "OpenGLIndexBuffer.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLShader.h"
namespace SPW
{
    class OpenGLBackEnd: public RenderBackEndI
    {
        void Init() override;
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;
        void DrawElement(std::shared_ptr<VertexBufferI>& vertexBuffer , std::shared_ptr<IndexBuffer>& indexBuffer) override;
        //depth
        virtual void DepthTest(bool enable);
        virtual void DepthMask(bool maskFlag);
        virtual void DepthFunc(DepthComp comp);
        //cull
        void Cull(int32_t Bit) override;
        void CullFrontOrBack(bool bFront) override;

        //for draw
        std::shared_ptr<IndexBuffer> createIndexBuffer(std::vector<unsigned int> indices) final
        {
            return std::make_shared<OpenGLIndexBuffer>(indices);
        }

        std::shared_ptr<VertexBufferI> createVertexBuffer() final
        {
            std::shared_ptr<VertexBufferI> VB = std::make_shared<OpenGLVertexBuffer>();
            VB->GenVertexArray();
            VB->GenVertexBuffer();
            return VB;
        }

        std::shared_ptr<Shader>createShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)final
        {
            return std::make_shared<OpenGLShader>(name,vertexSrc,fragmentSrc);
        }
    };
}



#endif //SPARROW_OPENGLBACKEND_H

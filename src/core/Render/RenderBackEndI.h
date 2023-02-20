//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_RENDERBACKENDI_H
#define SPARROW_RENDERBACKENDI_H

#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
namespace SPW
{
    enum class DepthComp
    {
        EQUAL,
        LEQUAL,
        LESS
    };
    class RenderBackEndI
    {
    public:
        enum class RenderAPIType
        {
            None = 0,
            OpenGL,
        };
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;
        virtual void DrawElement(const std::shared_ptr<VertexBufferI>& vertexBuffer ,const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;
        //depth
        virtual void DepthTest(bool enable) = 0;
        virtual void DepthMask(bool maskFlag) = 0;
        virtual void DepthFunc(DepthComp comp) = 0;
        //cull
        virtual void Cull(int32_t Bit) = 0;
        virtual void CullFrontOrBack(bool bFront) = 0;

        //creat structure;
        virtual std::shared_ptr<IndexBuffer> createIndexBuffer(std::vector<unsigned int> indices) = 0;
        virtual std::shared_ptr<VertexBufferI> createVertexBuffer() = 0;
    public:
        virtual void Init() = 0;
        static RenderAPIType getCurrent() { return currentRenderAPI;}
        virtual ~RenderBackEndI() = default;
    private:
        static inline RenderAPIType currentRenderAPI = RenderAPIType::OpenGL;

    };

}

#endif //SPARROW_RENDERBACKENDI_H

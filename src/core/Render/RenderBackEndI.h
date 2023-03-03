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
#include <map>
#include <string>
#include "Render/shader.h"
#include "Render/Material.h"
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
        virtual void DrawElement(std::shared_ptr<VertexBufferI>& vertexBuffer ,std::shared_ptr<IndexBuffer>& indexBuffer) = 0;
        //depth
        virtual void DepthTest(bool enable) = 0;
        virtual void DepthMask(bool maskFlag) = 0;
        virtual void DepthFunc(DepthComp comp) = 0;
        //cull
        virtual void Cull(int32_t Bit) = 0;
        virtual void CullFrontOrBack(bool bFront) = 0;

        //create texture
        virtual void BindTexture(std::shared_ptr<Shader>shader,std::shared_ptr<Material> material) = 0;

        //creat structure;
        virtual std::shared_ptr<IndexBuffer> createIndexBuffer(std::vector<unsigned int> indices) = 0;
        virtual std::shared_ptr<VertexBufferI> createVertexBuffer() = 0;
        virtual std::shared_ptr<Shader>createShader(const ShaderHandle &handle) = 0;
        std::shared_ptr<Shader> getShader(const ShaderHandle &handle)
        {
            if(shaderMap.find(handle)!=shaderMap.end())
            {
                return shaderMap[handle];
            }
            else
            {
                shaderMap[handle] = createShader(handle);
                return shaderMap[handle];
            }

        }
    public:
        virtual void Init() = 0;
        virtual ~RenderBackEndI() = default;
    protected:
        ShaderTable shaderMap;
    };

}

#endif //SPARROW_RENDERBACKENDI_H

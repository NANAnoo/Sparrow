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
#include "OpenGLCubeMap.h"
namespace SPW
{


    class OpenGLBackEnd: public RenderBackEndI
    {
    public:
        void Init() override;
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        void SetClearColor(const glm::vec4 color) override;
        void Clear() override;
        void DrawElement(std::shared_ptr<VertexBufferI>& vertexBuffer , std::shared_ptr<IndexBuffer>& indexBuffer) override;
        //depth
        void DepthTest(bool enable) override;
        void DepthMask(bool maskFlag) override;
        void DepthFunc(DepthComp comp) override;
        //cull
        void Cull(int32_t Bit) override;
        void CullFrontOrBack(bool bFront) override;
        void loadShaderLib(std::string libPath) override;

        //Texture
        void BindTexture(std::shared_ptr<Shader>shader,std::shared_ptr<Material>material) final;
        void InitSkyBox() final;
        void SetSkyBox(std::vector<std::string>& faces) final;
        void drawSkyBox(glm::mat4& V,glm::mat4& P) final;

        // uniform buffer
        void initStorageBuffer(std::shared_ptr<StorageBuffer> ub) final;

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

        std::shared_ptr<Shader>createShader( const ShaderHandle &handle)final
        {
            return std::make_shared<OpenGLShader>(handle);
        }

        std::shared_ptr<FrameBuffer> creatSenceFrameBuffer() final;
        void drawInTexture(SPW::PostProcessingEffects effect = SPW::PostProcessingEffects::None) final;
    private:
        
        unsigned int quadVAO, quadVBO;
        unsigned int skyboxVAO,skyboxVBO;
        std::shared_ptr<OpenGLCubeMap> cubemapTexture = nullptr;

        std::shared_ptr<Shader> skyShader = nullptr;
    };
}



#endif //SPARROW_OPENGLBACKEND_H

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
#include "Render/FrameBuffer.h"
#include "StorageBuffer.h"
#include "AttachmentTexture.h"
namespace SPW
{
    enum class DepthComp
    {
        EQUAL,
        LEQUAL,
        LESS
    };

    enum class PostProcessingEffects
    {
        None=0,
        Gauss,
        FXAA
    };

    class RenderGraph;

    class RenderBackEndI
    {
    public:
        std::shared_ptr<FrameBuffer> scenceFrameBuffer = nullptr;
        std::vector<std::shared_ptr<FrameBuffer>> shadowFrameBuffers;
        unsigned int depthTextureArray;
    public:
        enum class RenderAPIType
        {
            None = 0,
            OpenGL,
        };

        virtual std::shared_ptr<RenderGraph> createRenderGraph() = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SetClearColor(const glm::vec4 color) = 0;
        virtual void Clear() = 0;
        virtual void ClearColor() = 0;
        virtual void ClearDepth() = 0;
        virtual void DrawElement(std::shared_ptr<VertexBufferI>& vertexBuffer ,std::shared_ptr<IndexBuffer>& indexBuffer) = 0;
        //depth
        virtual void DepthTest(bool enable) = 0;
        virtual void DepthMask(bool maskFlag) = 0;
        virtual void DepthFunc(DepthComp comp) = 0;
        //cull
        virtual void Cull(int32_t Bit) = 0;
        virtual void CullFrontOrBack(bool bFront) = 0;

        //create texture
        virtual void BindTexture(std::shared_ptr<Shader> shader,std::shared_ptr<Material> material) = 0;
        virtual void InitSkyBox()=0;
        virtual void SetSkyBox(std::vector<std::string>& faces)=0;
        virtual void drawSkyBox(glm::mat4& V,glm::mat4& P)=0;

        // uniform buffer
        virtual void initStorageBuffer(std::shared_ptr<StorageBuffer> ubo) = 0;
        // bind texture at slot with a file path
        virtual void BindImageTex(std::string path, int slot) = 0;
        virtual void BindCubeMap(std::vector<std::string> paths, int slot) = 0;

        //frambuffer
        virtual std::shared_ptr<FrameBuffer> createFrameBuffer()=0;
        virtual std::shared_ptr<FrameBuffer> creatSenceFrameBuffer()=0;
        virtual void creatShadowFrameBuffer(unsigned int num)=0;
        virtual void setUpShadowArray(unsigned  int num) = 0;
        virtual void drawInTexture(SPW::PostProcessingEffects effect = SPW::PostProcessingEffects::None)=0;
        virtual void drawInQuad() = 0;

        virtual void dispatchCompute(unsigned int x, unsigned int y, unsigned int z) = 0;

        //creat structure;
        virtual std::shared_ptr<IndexBuffer> createIndexBuffer(std::vector<unsigned int> indices) = 0;
        virtual std::shared_ptr<VertexBufferI> createVertexBuffer() = 0;
        virtual std::shared_ptr<Shader>createShader(const ShaderHandle &handle) = 0;
        // 
        virtual void loadShaderLib(std::string libPath) = 0;
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

        // attachment textures
        virtual std::shared_ptr<AttachmentTexture> createAttachmentTexture() = 0;
        virtual std::shared_ptr<AttachmentTextureArray> createAttachmentTextureArray() = 0;
        virtual std::shared_ptr<AttachmentTextureCube> createAttachmentTextureCube() = 0;
        virtual std::shared_ptr<AttachmentTextureCubeArray> createAttachmentTextureCubeArray() = 0;
    public:
        virtual void Init() = 0;
        virtual ~RenderBackEndI() = default;
    protected:
        ShaderTable shaderMap;
    };

}

#endif //SPARROW_RENDERBACKENDI_H

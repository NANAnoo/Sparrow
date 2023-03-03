//
// Created by sgsdi on 2023/2/17.
//

#include "OpenGLBackEnd.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"
#include "OpenGLTextureManager.h"
#include "OpenGLTexture2D.h"
#include "OpenGLFrameBuffer.h"
#include <fstream>
float quadVertices[] =
{
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
 };

namespace SPW
{

    void OpenGLBackEnd::Init()
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        std::string shader_lib = "./resources/shaders/baselib/structure.glsl";

        std::stringstream ss;
        std::ifstream fs(shader_lib);
        ss << fs.rdbuf();
        auto code = ss.str();
        shader_lib = "/structure.glsl";
        glNamedStringARB(GL_SHADER_INCLUDE_ARB, shader_lib.size(), shader_lib.c_str(),
                         code.size(), code.c_str());

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    void OpenGLBackEnd::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }

    void OpenGLBackEnd::SetClearColor(const glm::vec4 color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLBackEnd::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void OpenGLBackEnd::DrawElement(std::shared_ptr<VertexBufferI> &vertexBuffer,
                                    std::shared_ptr<IndexBuffer> &indexBuffer)
    {
        indexBuffer->Bind();
        vertexBuffer->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indexBuffer->size), GL_UNSIGNED_INT, 0);
//        indexBuffer->UnBind();
        vertexBuffer->UnBind();
    }


    void OpenGLBackEnd::DepthTest(bool enable)
    {
        if(enable) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
    }

    void OpenGLBackEnd::DepthMask(bool maskFlag)
    {
        if (maskFlag) glDepthMask(GL_TRUE);
        else glDepthMask(GL_FALSE);
    }

    void OpenGLBackEnd::DepthFunc(DepthComp comp)
    {

    }

    void OpenGLBackEnd::Cull(int32_t Bit)
    {
        if (Bit) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
    }

    void OpenGLBackEnd::CullFrontOrBack(bool bFront)
    {
        if (bFront) glCullFace(GL_FRONT);
        else glCullFace(GL_BACK);
    }

    void OpenGLBackEnd::BindTexture(std::shared_ptr<Shader> shader, std::shared_ptr<Material>material)
    {
        shader->Bind();
        //albeo map
        if(material->TextureMap.find(TextureType::Albedo)!=material->TextureMap.end())
        {
            std::string path = material->TextureMap[TextureType::Albedo];
            std::shared_ptr<OpenGLtexture2D> AlbedoMap =
                    OpenGLTextureManager::getInstance()->getOpenGLtexture2D(path);
            shader->SetUniformValue<int>("albedoMap",0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, AlbedoMap->ID);
        }
        shader->SetUniformValue<float>("diffusion", 0.4);
        shader->SetUniformValue<float>("shininess", 0.3);
        shader->SetUniformValue<float>("lambertin", 0.3);
        shader->SetUniformValue<float>("specularPower", 50);

    }
    std::shared_ptr<FrameBuffer> OpenGLBackEnd::creatSenceFrameBuffer()
    {
        scenceFrameBuffer = std::make_shared<OpenGLFrameBuffer>();
        return scenceFrameBuffer;
    }

    void OpenGLBackEnd::drawInTexture(SPW::PostProcessingEffects effect)
    {
        SPW::ShaderHandle handle;
        if(effect==SPW::PostProcessingEffects::None)
        {
            handle = SPW::ShaderHandle({
                                               "drawIntexture",
                                               "./resources/shaders/screen.vert",
                                               "./resources/shaders/screen.frag"
                                       });
        }
        else if(effect==SPW::PostProcessingEffects::Gauss)
        {
            handle = SPW::ShaderHandle({
                                               "drawIntexture",
                                               "./resources/shaders/screen.vert",
                                               "./resources/shaders/Gauss.frag"
                                       });
        }
        std::shared_ptr<Shader> screenShader = this->getShader(handle);
        screenShader->Bind();
        screenShader->SetUniformValue<int>("screenTexture",0);
        glBindVertexArray(quadVAO);
        scenceFrameBuffer->drawinTexture();
        glBindVertexArray(0);
    }
}

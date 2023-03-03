//
// Created by sgsdi on 2023/2/17.
//

#include "OpenGLBackEnd.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"
#include "OpenGLTextureManager.h"
#include "OpenGLTexture2D.h"

#include <fstream>

namespace SPW
{

    void OpenGLBackEnd::Init()
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        std::string shader_lib = "./resources/shaders/structure.glsl";

        const GLubyte* byteGlVersion = glGetString(GL_VERSION);
        const GLubyte* byteGlVendor = glGetString(GL_VENDOR);
        const GLubyte* byteGlRenderer = glGetString(GL_RENDERER);
        const GLubyte* byteSLVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

        std::stringstream ss;
        std::ifstream fs(shader_lib);
        ss << fs.rdbuf();
        auto code = ss.str();
        shader_lib = "/structure.glsl";
        glNamedStringARB(GL_SHADER_INCLUDE_ARB, shader_lib.size(), shader_lib.c_str(),
                         code.size(), code.c_str());
    }

    void OpenGLBackEnd::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }

    void OpenGLBackEnd::SetClearColor(const glm::vec4 &color)
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
        //albeo map
        if(material->TextureMap.find(TextureType::Albedo)!=material->TextureMap.end())
        {
            std::string path = material->TextureMap[TextureType::Albedo];
            std::shared_ptr<OpenGLtexture2D> AlbedoMap =
                    OpenGLTextureManager::getInstance()->getOpenGLtexture2D(path);
            shader->Bind();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, AlbedoMap->ID);
            shader->SetUniformValue<int>("albedoMap",0);
        }

    }
}

//
// Created by sgsdi on 2023/2/17.
//

#include "OpenGLAPI.h"
namespace SPW
{

    void OpenGLAPI::Init()
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

    }

    void OpenGLAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }

    void OpenGLAPI::SetClearColor(const glm::vec4 &color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void OpenGLAPI::DrawElement(const std::shared_ptr<VertexBufferI> &vertexBuffer,
                                const std::shared_ptr<IndexBuffer> &indexBuffer)
    {
        indexBuffer->Bind();
        vertexBuffer->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indexBuffer->size), GL_UNSIGNED_INT, 0);
        //indexBuffer->UnBind();
        //vertexBuffer->UnBind();
    }


    void OpenGLAPI::DepthTest(bool enable)
    {
        if(enable) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
    }

    void OpenGLAPI::DepthMask(bool maskFlag)
    {
        if (maskFlag) glDepthMask(GL_TRUE);
        else glDepthMask(GL_FALSE);
    }

    void OpenGLAPI::DepthFunc(DepthComp comp)
    {

    }

    void OpenGLAPI::Cull(int32_t Bit)
    {
        if (Bit) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
    }

    void OpenGLAPI::CullFrontOrBack(bool bFront)
    {
        if (bFront) glCullFace(GL_FRONT);
        else glCullFace(GL_BACK);
    }
}

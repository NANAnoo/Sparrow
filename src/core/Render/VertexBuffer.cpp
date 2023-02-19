//
// Created by sgsdi on 2023/2/17.
//

#include "VertexBuffer.h"
#include "RenderAPII.h"
#include "Platforms/OPENGL/OpenGLVertexBuffer.h"
namespace SPW
{
    std::shared_ptr<VertexBufferI> VertexBufferI::CreateVertexBuffer()
    {
        switch (RenderAPII::getCurrent())
        {
            case RenderAPII::RenderAPIType::None: return nullptr;
            case RenderAPII::RenderAPIType::OpenGL: return std::make_shared<OpenGLVertexBuffer>();

        }
    }



}
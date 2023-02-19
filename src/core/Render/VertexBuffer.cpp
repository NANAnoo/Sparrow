//
// Created by sgsdi on 2023/2/17.
//

#include "VertexBuffer.h"
#include "RenderBackEndI.h"
#include "Platforms/OPENGL/OpenGLVertexBuffer.h"
namespace SPW
{
    std::shared_ptr<VertexBufferI> VertexBufferI::CreateVertexBuffer()
    {
        switch (RenderBackEndI::getCurrent())
        {
            case RenderBackEndI::RenderAPIType::None: return nullptr;
            case RenderBackEndI::RenderAPIType::OpenGL: return std::make_shared<OpenGLVertexBuffer>();

        }
    }



}
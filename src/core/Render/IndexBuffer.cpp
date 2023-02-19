//
// Created by sgsdi on 2023/2/17.
//

#include "IndexBuffer.h"
#include "Platforms/OPENGL/OpenGLIndexBuffer.h"

namespace SPW
{
    std::shared_ptr<IndexBuffer> IndexBuffer::GenIndexBuffer(std::vector<unsigned int>& indices)
    {
        switch (RenderAPII::getCurrent())
        {
            case RenderAPII::RenderAPIType::OpenGL:  return std::make_shared<OpenGLIndexBuffer>(indices);
        }
    }

}

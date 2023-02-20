//
// Created by sgsdi on 2023/2/17.
//

#include "GraphicsContext.h"
#include "RenderBackEndI.h"
#include <assert.h>
#include "Platforms/OPENGL/OpenGLContext.h"
#include <GLFW/glfw3.h>

namespace SPW
{
    std::shared_ptr<GraphicsContext> GraphicsContext::Create(void* window)
    {
        switch (RenderBackEndI::getCurrent())
        {
            case RenderBackEndI::RenderAPIType::None:
                assert("RenderAPI::None is not supported!");
                return nullptr;
            case RenderBackEndI::RenderAPIType::OpenGL:
                return std::make_shared<OpenGLContext>(static_cast<GLFWwindow*>(window));

        }
        return nullptr;
    }
}

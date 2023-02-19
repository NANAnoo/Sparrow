//
// Created by sgsdi on 2023/2/17.
//

#include "GraphicsContext.h"
#include "RenderAPII.h"
#include <assert.h>
#include "Platforms/OPENGL/OpenGLContext.h"
#include <GLFW/glfw3.h>

namespace SPW
{
    std::shared_ptr<GraphicsContext> GraphicsContext::Create(void* window)
    {
        switch (RenderAPII::getCurrent())
        {
            case RenderAPII::RenderAPIType::None:
                assert("RenderAPI::None is not supported!");
                return nullptr;
            case RenderAPII::RenderAPIType::OpenGL:
                return std::make_shared<OpenGLContext>(static_cast<GLFWwindow*>(window));

        }
        return nullptr;
    }
}

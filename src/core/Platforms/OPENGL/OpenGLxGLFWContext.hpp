//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_OPENGLXGLFWCONTEXT_HPP
#define SPARROW_OPENGLXGLFWCONTEXT_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Render/GraphicsContext.h"

namespace SPW
{
    class OpenGLxGLFWContext: public GraphicsContext
    {
    public:
        explicit OpenGLxGLFWContext(GLFWwindow *handle) {
            contextHandle = handle;
        }
        void Init() override {
            glfwMakeContextCurrent(contextHandle);
            int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
            if(!status)
                assert("glad init false");
            glfwSwapInterval(1);
        }

        void SwapBuffers() override {
            glfwSwapBuffers(contextHandle);
        }

    protected:
        GLFWwindow *contextHandle;
    };

}




#endif //SPARROW_OPENGLXGLFWCONTEXT_HPP

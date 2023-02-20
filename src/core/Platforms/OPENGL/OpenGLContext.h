//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_OPENGLCONTEXT_H
#define SPARROW_OPENGLCONTEXT_H

#include "Render/GraphicsContext.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace SPW
{
    class OpenGLContext:public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);

        void Init() final;
        void SwapBuffers() final;

    private:
        GLFWwindow* mWindowHandle;
    };

}




#endif //SPARROW_OPENGLCONTEXT_H

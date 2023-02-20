//
// Created by sgsdi on 2023/2/17.
//

#include "OpenGLContext.h"
#include "glad/glad.h"
#include "assert.h"
namespace SPW
{
    OpenGLContext::OpenGLContext(GLFWwindow *windowHandle):mWindowHandle(windowHandle)
    {
        if(windowHandle==nullptr)
            assert("windowhandle is NULL");
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(mWindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        if(!status)
            assert("glad init false");
    }
    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(this->mWindowHandle);

    }
}

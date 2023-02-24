//
// Created by sgsdi on 2023/2/24.
//

#include "OpenGLFrameBuffer.h"
#include "glad/glad.h"
#include <iostream>

namespace SPW
{

    void OpenGLFrameBuffer::genFrameBuffer()
    {
        glGenFramebuffers(1, &framebufferId);
    }

    void OpenGLFrameBuffer::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER,framebufferId);
    }

    void OpenGLFrameBuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }

    void OpenGLFrameBuffer::deleteFrameBuffer()
    {
        glDeleteFramebuffers(1,&framebufferId);
    }

    void OpenGLFrameBuffer::AttachColorTexture(unsigned int width, unsigned int height, unsigned int slot)
    {
        glGenTextures(1, &textureColorbufferId);
        glBindTexture(GL_TEXTURE_2D, textureColorbufferId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot,GL_TEXTURE_2D, textureColorbufferId, 0);
    }

    void OpenGLFrameBuffer::AttachDepthTexture()
    {
        return;
    }

    void OpenGLFrameBuffer::AttachColorRenderBuffer(unsigned int width,unsigned int height,unsigned int slot)
    {
        glGenRenderbuffers(1, &rboId);
        glBindRenderbuffer(GL_RENDERBUFFER, rboId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_R32I, width, height);
        //glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_RENDERBUFFER, rboId);
    }

    void OpenGLFrameBuffer::AttachDepthRenderBuffer(unsigned int width,unsigned int height)
    {
        glGenRenderbuffers(1, &rboId);
        glBindRenderbuffer(GL_RENDERBUFFER, rboId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        //glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);
    }

    void OpenGLFrameBuffer::CheckFramebufferStatus()
    {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    void OpenGLFrameBuffer::drawinTexture()
    {
        glBindTexture(GL_TEXTURE_2D, textureColorbufferId);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
} // SPW
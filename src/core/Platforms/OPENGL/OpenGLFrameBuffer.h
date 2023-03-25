//
// Created by sgsdi on 2023/2/24.
//

#ifndef SPARROW_OPENGLFRAMEBUFFER_H
#define SPARROW_OPENGLFRAMEBUFFER_H
#include "Render/FrameBuffer.h"
namespace SPW
{

    class OpenGLFrameBuffer :public FrameBuffer
    {
    public:
        explicit OpenGLFrameBuffer()=default;
        void genFrameBuffer()final;
        void bind()final;
        void unbind()final;
        void deleteFrameBuffer()final;
        void AttachColorTexture(unsigned int width,unsigned int height,unsigned int slot)final;
        void AttachDepthTexture()final;
        void AttachDepthTexture3D(unsigned int array,unsigned int depth) final;
        void AttachColorRenderBuffer(unsigned int width,unsigned int height,unsigned int slot)final;
        void AttachDepthRenderBuffer(unsigned int width,unsigned int height)final;
        void CheckFramebufferStatus()final;
        void drawinTexture() final;
    private:
        unsigned int framebufferId;
        unsigned int rboId;
    public:
        unsigned int textureColorbufferId;

    };

} // SPW

#endif //SPARROW_OPENGLFRAMEBUFFER_H

//
// Created by sgsdi on 2023/2/24.
//

#ifndef SPARROW_FRAMEBUFFER_H
#define SPARROW_FRAMEBUFFER_H
namespace SPW
{
    class FrameBuffer
    {
    public:
        explicit FrameBuffer() = default;
        virtual void genFrameBuffer()=0;
        virtual void bind()=0;
        virtual void unbind()=0;
        virtual void deleteFrameBuffer()=0;
        virtual void AttachColorTexture(unsigned int width,unsigned int height,unsigned int slot)=0;
        virtual void AttachDepthTexture()=0;
        virtual void AttachColorRenderBuffer(unsigned int width,unsigned int height,unsigned int slot)=0;
        virtual void AttachDepthRenderBuffer(unsigned int width,unsigned int height)=0;
        virtual void CheckFramebufferStatus()=0;
        virtual void drawinTexture()=0;
    };
}

#endif //SPARROW_FRAMEBUFFER_H
#pragma once

#include "PipeLine.hpp"
#include "FrameBuffer.h"

namespace SPW {
    class AttachmentTexture {
        public:
            explicit AttachmentTexture()=default;
            virtual void genTexture(unsigned int width,
                                    unsigned int height,
                                    ColorAttachmentFormat format) = 0;
            virtual void attach(unsigned int index) = 0;
            virtual unsigned int use(unsigned int slot)=0;
            virtual void unbind()=0;
            virtual void deleteTexture()=0;
    };

    class AttachmentTextureArray {
        public:
            explicit AttachmentTextureArray()=default;
            virtual void genTexture(unsigned int width,
                                    unsigned int height, 
                                    unsigned int count,
                                    ColorAttachmentFormat format) = 0;
            virtual void attach(unsigned int index, unsigned int layer) = 0;
            virtual unsigned int use(unsigned int slot)=0;
            virtual void unbind()=0;
            virtual void deleteTexture()=0;
    };

    class AttachmentTextureCube {
        public:
            explicit AttachmentTextureCube()=default;
            virtual void genTexture(unsigned int width,
                                    unsigned int height, 
                                    ColorAttachmentFormat format) = 0;
            virtual void attach(unsigned int index, unsigned int face) = 0;
            virtual unsigned int use(unsigned int slot)=0;
            virtual void unbind()=0;
            virtual void deleteTexture()=0;
    };

    class AttachmentTextureCubeArray {
        public:
            explicit AttachmentTextureCubeArray()=default;
            virtual void genTexture(unsigned int width,
                                    unsigned int height, 
                                    unsigned int count,
                                    ColorAttachmentFormat format) = 0;
            virtual void attach(unsigned int index, unsigned int layer, unsigned int face) = 0;
            virtual unsigned int use(unsigned int slot)=0;
            virtual void unbind()=0;
            virtual void deleteTexture()=0;
    };
}
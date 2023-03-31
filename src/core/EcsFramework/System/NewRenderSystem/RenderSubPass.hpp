#include "Render/RenderBackEndI.h"
#include "Render/PipeLine.hpp"
#include "Render/AttachmentTexture.h"

#include "EcsFramework/Component/MeshComponent.hpp"

namespace SPW {
    struct AttachmentPack {
        public:
            std::shared_ptr<SPW::AttachmentTexture> texture = nullptr;
            std::shared_ptr<SPW::AttachmentTextureArray> textureArray = nullptr;
            std::shared_ptr<SPW::AttachmentTextureCube> textureCube = nullptr;
            std::shared_ptr<SPW::AttachmentTextureCubeArray> textureCubeArray = nullptr;
    };
    class SPWModelSubPass {
    public:
        SPWModelSubPass(const ModelSubPassConfig &config) {
            this->config = config;
        };

        void init(const std::shared_ptr<RenderBackEndI> &backend, unsigned int width, unsigned int height) {
            attchmentFrameBuffers.clear();
            attchmentTextures.clear();
            unsigned int frameWidth = config.frameWidth == 0 ? width : config.frameWidth;
            unsigned int frameHeight = config.frameHeight == 0 ? height : config.frameHeight;

            if (hasInternalFramebuffer()) {
                if (config.repeatType == NoRepeat) {
                    attchmentFrameBuffers.push_back(backend->createFrameBuffer());
                    attchmentFrameBuffers[0]->genFrameBuffer();
                    attchmentFrameBuffers[0]->bind();
                    unsigned int index = 0;
                    for (auto format : config.attachments) {
                        AttachmentPack texture;
                        if (config.outputType == ColorType) {
                            texture.texture = backend->createAttachmentTexture();
                            texture.texture->genTexture(frameWidth, frameHeight, format);
                            texture.texture->attach(index);
                        } else if (config.outputType == CubeMapType) {
                            texture.textureCube = backend->createAttachmentTextureCube();
                            texture.textureCube->genTexture(frameWidth, frameHeight, format);
                            // attch before draw
                            // texture.textureCube->attach(index, 0);
                        }
                        if (format != Depth) {
                            attchmentFrameBuffers[0]->AttachColorRenderBuffer(frameWidth,frameHeight,index++);
                        } else {
                            attchmentFrameBuffers[0]->AttachDepthRenderBuffer(frameHeight,frameHeight);
                        }
                        attchmentTextures.push_back(texture);
                    }
                    attchmentFrameBuffers[0]->CheckFramebufferStatus();
                    attchmentFrameBuffers[0]->unbind();
                } else {
                    // has repeat type, create frame buffer array
                    for (unsigned int i = 0; i < config.repeatCount; i ++) {
                        attchmentFrameBuffers.push_back(backend->createFrameBuffer());
                        attchmentFrameBuffers[i]->genFrameBuffer();
                        attchmentFrameBuffers[i]->bind();
                        unsigned int index = 0;
                        for (auto format : config.attachments) {
                            AttachmentPack texture;
                            if (config.outputType == ColorType) {
                                texture.textureArray = backend->createAttachmentTextureArray();
                                texture.textureArray->genTexture(frameWidth, frameHeight, config.repeatCount, format);
                                texture.textureArray->attach(index, i);
                            } else if (config.outputType == CubeMapType) {
                                texture.textureCubeArray = backend->createAttachmentTextureCubeArray();
                                texture.textureCubeArray->genTexture(frameWidth, frameHeight, config.repeatCount, format);
                                // attch before draw;
                            }
                            if (format != Depth) {
                                attchmentFrameBuffers[i]->AttachColorRenderBuffer(frameWidth,frameHeight,index++);
                            } else {
                                attchmentFrameBuffers[i]->AttachDepthRenderBuffer(frameHeight,frameHeight);
                            }
                            attchmentTextures.push_back(texture);
                        }
                        attchmentFrameBuffers[i]->CheckFramebufferStatus();
                        attchmentFrameBuffers[i]->unbind();
                    }

                }
            }
        }

        bool hasInternalFramebuffer() {
            return !config.attachments.empty();
        }

        ModelSubPassConfig config;
        // store output frame buffer
        std::vector<std::shared_ptr<FrameBuffer>> attchmentFrameBuffers;
        // store output texture
        std::vector<AttachmentPack> attchmentTextures;
    };

    class SPWImageSubPass {
    public:
        SPWImageSubPass(const ImageSubPassConfig &config) {
            this->config = config;
        };

        void init(const std::shared_ptr<RenderBackEndI> &backend, unsigned int width, unsigned int height) {
            unsigned int frameWidth = config.frameWidth == 0 ? width : config.frameWidth;
            unsigned int frameHeight = config.frameHeight == 0 ? height : config.frameHeight;

            if (hasInternalFramebuffer()) {
                internalFrameBuffer = backend->createFrameBuffer();
                internalFrameBuffer->genFrameBuffer();
                internalFrameBuffer->bind();
                unsigned int index = 0;
                for (auto format : config.attachments) {
                    AttachmentPack texture;
                    if (config.outputType == ColorType) {
                        texture.texture = backend->createAttachmentTexture();
                        texture.texture->genTexture(frameWidth, frameHeight, format);
                        texture.texture->attach(index);
                    } else if (config.outputType == CubeMapType) {
                        texture.textureCube = backend->createAttachmentTextureCube();
                        texture.textureCube->genTexture(frameWidth, frameHeight, format);
                        // attch before draw
                        // texture.textureCube->attach(index, 0);
                    }
                    if (format != Depth) {
                        internalFrameBuffer->AttachColorRenderBuffer(frameWidth,frameHeight,index++);
                    } else {
                        internalFrameBuffer->AttachDepthRenderBuffer(frameHeight,frameHeight);
                    }
                    attchmentTextures.push_back(texture);
                }
                internalFrameBuffer->CheckFramebufferStatus();
                internalFrameBuffer->unbind();
            }
        }

        bool hasInternalFramebuffer() {
            return !config.attachments.empty();
        }

        ImageSubPassConfig config;
        std::shared_ptr<FrameBuffer> internalFrameBuffer;
        // store output texture
        std::vector<AttachmentPack> attchmentTextures;
    };
}
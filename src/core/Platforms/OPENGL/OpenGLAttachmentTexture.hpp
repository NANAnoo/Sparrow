#pragma once

#include "Render/AttachmentTexture.h"
#include "glad/glad.h"

namespace SPW {
    inline void getTypeFromAttachmentFormat(ColorAttachmentFormat format, unsigned int &innerFormat, unsigned int &formatType, unsigned int &dataType) {
        switch (format) {
            case RGBA8:
                formatType = GL_RGBA8;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case RGBA16:
                innerFormat = GL_RGBA16F;
                formatType = GL_RGBA;
                dataType = GL_HALF_FLOAT;
                break;
            case RGBA32:
                formatType = GL_RGBA;
                innerFormat = GL_RGBA32F;
                dataType = GL_FLOAT;
                break;
            case Depth:
                innerFormat = GL_DEPTH_COMPONENT24;
                formatType = GL_DEPTH_COMPONENT;
                dataType = GL_FLOAT;
                break;
            case R8:
                formatType = GL_R8;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case RG8:
                formatType = GL_RG8;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case RGB8:
                formatType = GL_RGB8;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case R16:
                formatType = GL_R16F;
                dataType = GL_HALF_FLOAT;
                break;
            case RG16:
                formatType = GL_RG16F;
                dataType = GL_HALF_FLOAT;
                break;
            case RGB16:
                formatType = GL_RGB;
                innerFormat = GL_RGB16F;
                dataType = GL_FLOAT;
                break;
            case R32:
                formatType = GL_R;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case RG32:
                formatType = GL_RG;
                dataType = GL_UNSIGNED_BYTE;
                break;
            case RGB32:
                innerFormat = GL_RGB;
                formatType = GL_RGB;
                dataType = GL_UNSIGNED_BYTE;
                break;
            }
    }
    class OpenGLAttachmentTexture : public AttachmentTexture{
        public:
            explicit OpenGLAttachmentTexture() = default;
            ~ OpenGLAttachmentTexture() {
                deleteTexture();
            }
            void genTexture(unsigned int width,
                            unsigned int height, 
                            ColorAttachmentFormat format) override
            {
                // use switch trasnlate ColorAttachmentFormat::format to opengl format
                getTypeFromAttachmentFormat(format, innerFormat, formatType, dataType);
                glGenTextures(1, &m_textureID);
                glBindTexture(GL_TEXTURE_2D, m_textureID);
                //glTexStorage2D(GL_TEXTURE_2D, 1, formatType, width, height);
                glTexImage2D(GL_TEXTURE_2D, 0, innerFormat, width, height, 0, formatType, dataType, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                // set border as [1, 1, 1, 1] when format is depth
                if (formatType == GL_DEPTH_COMPONENT) {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
                    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
                }
                // glBindTexture(GL_TEXTURE_2D, 0);
            }

            // atach to framebuffer on index
            void attach(unsigned int index) override
            {   
                if (formatType == GL_DEPTH_COMPONENT) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textureID, 0);
                } else {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, m_textureID, 0);
                }
            }
            // bind to slot in shader
            unsigned int use(unsigned int slot) override
            {
                glActiveTexture(GL_TEXTURE0 + slot);
                glBindTexture(GL_TEXTURE_2D, m_textureID);
                return slot + 1;
            }
            void unbind() override
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            void deleteTexture() override
            {
                glBindTexture(GL_TEXTURE_2D, 0);
                glDeleteTextures(1, &m_textureID);
            }


            unsigned int m_textureID = 0;
            unsigned int innerFormat = 0;
            unsigned int formatType = 0;
            unsigned int dataType = 0;
    };

    class OpenGLAttachmentTextureArray : public AttachmentTextureArray {
        public:
             explicit OpenGLAttachmentTextureArray() = default;
            ~ OpenGLAttachmentTextureArray() {
                deleteTexture();
            }

            void genTexture(unsigned int width,
                            unsigned int height, 
                            unsigned int count,
                            ColorAttachmentFormat format) override
            {
                getTypeFromAttachmentFormat(format, innerFormat, formatType, dataType);
                glGenTextures(1, &m_textureID);
                glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
                //glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, formatType, width, height, count);
                glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, innerFormat, width, height, count, 0, formatType, dataType, nullptr);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                // set border as [1, 1, 1, 1] when format is depth
                if (formatType == GL_DEPTH_COMPONENT) {
                    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
                    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
                }
                this->m_count = count;
            }

            void attach(unsigned int index, unsigned int layer) override {
                if (formatType == GL_DEPTH_COMPONENT) {
                    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_textureID, 0, layer);
                } else {
                    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, m_textureID, 0, layer);
                }
            }

            unsigned int use(unsigned int slot) override
            {
                glActiveTexture(GL_TEXTURE0 + slot);
                glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
                return slot + 1;
            }

            void unbind() override
            {
                glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
            }

            void deleteTexture() override
            {
                glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
                glDeleteTextures(1, &m_textureID);
            }

        private:
            unsigned int m_textureID = 0;
            unsigned int m_count = 0;
            unsigned int innerFormat = 0;
            unsigned int formatType = 0;
            unsigned int dataType = 0;
    };

    class OpenGLAttachmentTextureCube : public AttachmentTextureCube {
        public:
            explicit OpenGLAttachmentTextureCube() = default;
            void genTexture(unsigned int width,
                            unsigned int height, 
                            ColorAttachmentFormat format) override
            {
                getTypeFromAttachmentFormat(format, innerFormat, formatType, dataType);
                glGenTextures(1, &m_textureID);
                glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
                for (unsigned int i = 0; i < 6; i++) {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, innerFormat, width, height, 0, formatType, dataType, nullptr);
                }
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                // set border as [1, 1, 1, 1] when format is depth
                if (formatType == GL_DEPTH_COMPONENT) {
                    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
                    glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);
                }

                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            }

            void attach(unsigned int index, unsigned int face) override {
                if (formatType == GL_DEPTH_COMPONENT) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_textureID, 0);
                } else {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_textureID, 0);
                }
            }

            unsigned int use(unsigned int slot) override
            {
                glActiveTexture(GL_TEXTURE0 + slot);
                glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
                return slot + 1;
            }

            void unbind() override
            {
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            }

            void deleteTexture() override
            {
                glDeleteTextures(1, &m_textureID);
            }
        private:
            unsigned int m_textureID = 0;
            unsigned int innerFormat = 0;
            unsigned int formatType = 0;
            unsigned int dataType = 0;
    };

    class OpenGLAttachmentTextureCubeArray : public AttachmentTextureCubeArray {
        public:
            explicit OpenGLAttachmentTextureCubeArray() = default;
            void genTexture(unsigned int width,
                            unsigned int height, 
                            unsigned int count,
                            ColorAttachmentFormat format) override
            {
                getTypeFromAttachmentFormat(format, innerFormat, formatType, dataType);
                glGenTextures(1, &m_textureID);
                glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_textureID);
                //glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, formatType, width, height, count * 6);
                glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, innerFormat, width, height, count * 6,  0, formatType, dataType, nullptr);
                glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                // set border as [1, 1, 1, 1] when format is depth
                if (formatType == GL_DEPTH_COMPONENT) {
                    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
                    glTexParameterfv(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
                }
                glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);
                m_count = count;
            }

            void attach(unsigned int index, unsigned int layer, unsigned int face) override {
                GLint layerFaceIndex = layer * 6 + face;
                if (formatType == GL_DEPTH_COMPONENT) {
                    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_textureID, 0, layerFaceIndex);
                } else {
                    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, m_textureID, 0, layerFaceIndex);
                }
            }

            unsigned int use(unsigned int slot) override
            {
                glActiveTexture(GL_TEXTURE0 + slot);
                glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_textureID);
                return slot + 1;
            }

            void unbind() override
            {
                glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);
            }

            void deleteTexture() override
            {
                glDeleteTextures(1, &m_textureID);
            }

        private:
            unsigned int innerFormat = 0;
            unsigned int m_textureID = 0;
            unsigned int formatType = 0;
            unsigned int dataType = 0;
            unsigned int m_count = 0;
    };
}
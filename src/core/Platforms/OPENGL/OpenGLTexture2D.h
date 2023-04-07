//
// Created by sgsdi on 2023/2/22.
//

#ifndef SPARROW_OPENGLTEXTURE2D_H
#define SPARROW_OPENGLTEXTURE2D_H

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "IO/ResourceManager.h"

namespace SPW
{
    class OpenGLtexture2D
    {
    public:
        unsigned int ID;
    public:
        OpenGLtexture2D(std::string path)
        {
            glGenTextures(1, &ID);
            glBindTexture(GL_TEXTURE_2D, ID);
            // set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // load image, create texture and generate mipmaps
            int width, height, nrComponents;
            ResourceManager::getInstance()->LoadTextureScope(false, path,
                [this, path](int w, int h, int nrComponents, unsigned char *data) {
                    if (data)
                    {
                        GLenum format1,format2;
                        if (nrComponents == 1)
                        {
                            format1 = GL_RED;
                            format2 = GL_COMPRESSED_RED;
                        }
                        else if (nrComponents == 3)
                        {
                            format1 = GL_RGB;
                            format2 = GL_COMPRESSED_RGB;
                        }
                        else if (nrComponents == 4)
                        {
                            format1 = GL_RGBA;
                            format2 = GL_COMPRESSED_RGBA;
                        }
                        glBindTexture(GL_TEXTURE_2D, ID);
                        glTexImage2D(GL_TEXTURE_2D, 0, format2, w, h, 0, format1, GL_UNSIGNED_BYTE, data);
                        glGenerateMipmap(GL_TEXTURE_2D);

                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    } else {
                        std::cout << "Texture failed to load at path: " << path << std::endl;
                    }
            });
        }
        void realse()
        {
            glDeleteTextures(1,&ID);
        }
        ~OpenGLtexture2D()
        {
            realse();
        }
    };

}

#endif //SPARROW_OPENGLTEXTURE2D_H

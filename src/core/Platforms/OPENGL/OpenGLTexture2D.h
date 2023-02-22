//
// Created by sgsdi on 2023/2/22.
//

#ifndef SPARROW_OPENGLTEXTURE2D_H
#define SPARROW_OPENGLTEXTURE2D_H

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
            unsigned char* data= stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
            //TODO: CHANGE THESE CODE TO IO
            if (data)
            {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D, ID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
                std::cout << "Texture failed to load at path: " << path << std::endl;

            }
            stbi_image_free(data);
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

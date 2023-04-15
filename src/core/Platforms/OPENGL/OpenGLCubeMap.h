//
// Created by sgsdi on 2023/3/7.
//

#ifndef SPARROW_OPENGLCUBEMAP_H
#define SPARROW_OPENGLCUBEMAP_H

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Asset/ResourceManager/ResourceManager.h"
#include "Asset/AssetManager/AssetManager.h"

namespace SPW
{
    class OpenGLCubeMap
    {
    public:
        unsigned int ID;
    public:
        OpenGLCubeMap(std::vector<std::string> faces)
        {
            ID = AssetManager::LoadCubemapImages(faces);
            // glGenTextures(1, &ID);
            // glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
            // for (unsigned int i = 0; i < faces.size(); i++)
            // {
            //     ResourceManager::getInstance()->LoadTextureScope(false, faces[i].c_str(),
            //                                                      [this, i,&faces](int w, int h, int nrComponents, unsigned char *data)
            //     {
            //         if (data)
            //         {
            //             glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB,
            //                          GL_UNSIGNED_BYTE, data);
            //         } else
            //         {
            //             std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            //         }
            //     });
            // }
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }
        void realse()
        {
            glDeleteTextures(1,&ID);
        }
        ~OpenGLCubeMap()
        {
            realse();
        }
    };

}

#endif //SPARROW_OPENGLCUBEMAP_H

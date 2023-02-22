//
// Created by sgsdi on 2023/2/22.
//

#ifndef SPARROW_OPENGLTEXTUREMANAGER_H
#define SPARROW_OPENGLTEXTUREMANAGER_H

#include <string>
#include "OpenGLTexture2D.h"
#include "Utils/SingletonBase.h"
#include <map>
#include <string>

namespace SPW
{
    class OpenGLTextureManager : public SingletonBase<OpenGLTextureManager>
    {
    public:
        OpenGLTextureManager() = default;
        std::shared_ptr<OpenGLtexture2D> getOpenGLtexture2D(std::string& path)
        {
            if(OpenGLTextureMap.find(path)!=OpenGLTextureMap.end())
            {
                return OpenGLTextureMap[path];
            }
            OpenGLTextureMap[path] = std::make_shared<OpenGLtexture2D>(path);
            return OpenGLTextureMap[path];
        }

    private:
        std::map<std::string,std::shared_ptr<OpenGLtexture2D>> OpenGLTextureMap;
    };


}



#endif //SPARROW_OPENGLTEXTUREMANAGER_H

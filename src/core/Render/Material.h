//
// Created by sgsdi on 2023/2/22.
//

#ifndef SPARROW_MATERIAL_H
#define SPARROW_MATERIAL_H

#include <string>
#include <map>
namespace SPW
{
    enum class TextureType
    {
        Albedo = 0,
        Normal,
        Metalness,
        Roughness,
        AmbientOcclusion,
        Unknown = 18,
    };


    class Material
    {
    public:
        std::map<TextureType, std::string> TextureMap;

        void updateTexture(TextureType type,std::string texturePath)
        {
            if(texturePath=="")
            {
                TextureMap.erase(type);
                return;
            }

            TextureMap[type] = texturePath;
        }

    };
}

#endif //SPARROW_MATERIAL_H


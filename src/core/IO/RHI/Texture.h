/*
 * Created by elecfrog@du
 * 2023/02/16
 * Stay Empty for API specification
 */

#pragma once
#include "Definitions.h"
#include "Utils/MacroUtils.h"

namespace SPW
{
    class Texture
    {
        virtual void Bind(Count _slot = 0) const    = 0;
        virtual void Unbind(Count _slot = 0) const  = 0;
        virtual TextureType GetType() const         = 0;

    };

    class Texture2D : public Texture
    {        
        void Bind(Count _slot = 0) const   override;
        void Unbind(Count _slot = 0) const override;
        TextureType GetType() const        override;
    };

    class TextureCube : public Texture
    {        
        void Bind(Count _slot = 0) const   override;
        void Unbind(Count _slot = 0) const override;
        TextureType GetType() const        override;
    };

    // TODO: Design Other Dervied Texture Classes...
}
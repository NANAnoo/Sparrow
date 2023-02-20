//
// Created by 惠普 on 2023/2/20.
//

#ifndef SPARROW_AUDIOCOMPONENT_H
#define SPARROW_AUDIOCOMPONENT_H

#include "../ComponentI.h"
#include "fmod.hpp"
#include "string"

namespace SPW
{
    class AudioComponent : public ComponentI{
    public:
        AudioComponent() = default;
        AudioComponent(const AudioComponent&) = default;
        AudioComponent(const std::string& Path) : AudioPath(Path){}


        std::string AudioPath = "None";
        bool Play;

        FMOD::Sound* Sound;
        FMOD::Channel* Channel = nullptr;
    private:

    };
}
#endif //SPARROW_AUDIOCOMPONENT_H

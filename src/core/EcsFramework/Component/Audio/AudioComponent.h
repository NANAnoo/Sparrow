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
    struct AudioState{};
    class AudioComponent : public ComponentI{
    public:

        AudioComponent() = default;
        AudioComponent(const AudioComponent&) = default;
        AudioComponent(const std::string& Path) : AudioPath(Path){}

        int getTime(){ return  time;}
        void addTime(){time++;}

        std::string AudioPath = "None";

        bool isPlay;
        bool isPaused;
        //bool isPlayedOnAwake;
        bool isLoop;

        bool is3D;

        FMOD::Sound* Sound = nullptr;
        FMOD::Channel* Channel = nullptr;
        FMOD_MODE ModeType = FMOD_DEFAULT;
    private:

        //create time must less equal to 1
        int time = 0;
    };


}
#endif //SPARROW_AUDIOCOMPONENT_H

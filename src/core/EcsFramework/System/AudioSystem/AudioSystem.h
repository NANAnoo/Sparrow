//
// Created by 惠普 on 2023/3/3.
//

#ifndef SPARROW_AUDIOSYSTEM_H
#define SPARROW_AUDIOSYSTEM_H

#include "../SystemI.h"
#include "fmod.hpp"
#include "fmod.h"
#include "string"

namespace SPW
{

    class AudioSystem : public SystemI{

    public:
        AudioSystem(std::shared_ptr<Scene> &scene);
        virtual ~AudioSystem() = default;

        void initial() override;
        void beforeUpdate() override;
        void onUpdate(TimeDuration dt) override;
        void afterUpdate() override;
        void onStop() override;
    };
}

#endif //SPARROW_AUDIOSYSTEM_H

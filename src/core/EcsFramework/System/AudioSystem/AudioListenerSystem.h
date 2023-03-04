//
// Created by 惠普 on 2023/3/3.
//

#ifndef SPARROW_AUDIOLISTENERSYSTEM_H
#define SPARROW_AUDIOLISTENERSYSTEM_H

#include "../SystemI.h"
#include "fmod.hpp"
#include "fmod.h"
#include "string"

namespace SPW
{

    class AudioListenerSystem : public SystemI{

    public:
        explicit AudioListenerSystem(std::shared_ptr<Scene> &scene);
        virtual ~AudioListenerSystem() = default;

        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;
    };
}
#endif //SPARROW_AUDIOLISTENERSYSTEM_H

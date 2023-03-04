//
// Created by 惠普 on 2023/3/3.
//

#ifndef SPARROW_AUDIOSYSTEM_H
#define SPARROW_AUDIOSYSTEM_H

#include "../SystemI.h"
#include "fmod.hpp"
#include "fmod.h"
#include "string"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/Audio/AudioComponent.h"

namespace SPW
{
    using Music = std::tuple<IDComponent *,TransformComponent *,AudioComponent*>;
    class AudioSystem : public SystemI{

    public:
        explicit AudioSystem(std::shared_ptr<Scene> &scene);
        virtual ~AudioSystem() = default;

        void set3DModule(FMOD_MODE f);

        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;

    private:
        FMOD::System* mFmodSystem{};
        FMOD_SYSTEM* f_system{};


    };
}

#endif //SPARROW_AUDIOSYSTEM_H

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
#include <string>

namespace SPW
{
    using Music = std::tuple<IDComponent *,TransformComponent *,AudioComponent*>;
    class AudioSystem : public SystemI, public SPWSoundDelegateI {

    public:
        explicit AudioSystem(std::shared_ptr<Scene> &scene);
        virtual ~AudioSystem() = default;

        void manageSound();
        void matchSound();


        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;

        FMOD::Channel* playSound(const std::string &path, bool is3D, bool isLoop) override;

    private:
        FMOD::System* mFmodSystem;
        std::unordered_map<std::string, FMOD::Sound *> soundData;
    };
}

#endif //SPARROW_AUDIOSYSTEM_H

//
// Created by 惠普 on 2023/3/3.
//

#ifndef SPARROW_AUDIOLISTENERSYSTEM_H
#define SPARROW_AUDIOLISTENERSYSTEM_H

#include "../SystemI.h"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include <fmod.hpp>
#include <fmod.h>
#include "string"

namespace SPW
{
    using Listen = std::tuple<IDComponent *,TransformComponent *,AudioListener*>;
    class AudioListenerSystem : public SystemI{

    public:
        explicit AudioListenerSystem(std::shared_ptr<Scene> &scene);
        virtual ~AudioListenerSystem() = default;

        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;

    private:
        FMOD::System* ListenerSystem;
    };
}
#endif //SPARROW_AUDIOLISTENERSYSTEM_H

#include "AudioListenerSystem.h"
#include "./EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include <fmod.hpp>
#include <fmod.h>

namespace SPW{

    void AudioListenerSystem::initial() {
        FMOD::System_Create(&ListenerSystem);
        ListenerSystem->init(32,FMOD_INIT_NORMAL,0);
    }

    void AudioListenerSystem::beforeUpdate() {}

    void AudioListenerSystem::onUpdate(TimeDuration dt) {

        Listen uiListen;

        ComponentGroup<SPW::IDComponent,SPW::TransformComponent,SPW::AudioListener> ListenerGroup;

        locatedScene.lock()->forEachEntityInGroup(
                ListenerGroup,
                [this,&uiListen,&ListenerGroup](const Entity &en) {
                    auto trans = en.component<TransformComponent>();
                    auto player = en.component<AudioListener>();
                    FMOD_VECTOR listenerPos = { trans->position.x, trans->position.y, trans->position.z }; // 获取当前帧的监听器位置
                    FMOD_VECTOR listenerVel = { 0, 0, 0 }; // 获取当前帧的监听器速度
                    FMOD_VECTOR listenerForward = { trans->rotation.x, trans->rotation.y, trans->rotation.z }; // 获取当前帧的监听器前向方向
                    FMOD_VECTOR listenerUp = { 0, 1, 0 }; // 获取当前帧的监听器上向方向

                    ListenerSystem->set3DListenerAttributes(player->Listener_id, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
                    ListenerSystem->update();

                });
    }

    void AudioListenerSystem::afterUpdate() {}

    void AudioListenerSystem::onStop() {
        ListenerSystem->close();

        ListenerSystem->release();
    }

    AudioListenerSystem::AudioListenerSystem(std::shared_ptr<Scene> &scene) : SystemI(scene) {

    }
}
#include "AudioSystem.h"
#include "./EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/Scene.hpp"
#include "./EcsFramework/Component/Audio/AudioComponent.h"
#include "./EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/TransformComponent.hpp"
#include <fmod.hpp>
#include <string>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>

namespace SPW {

    AudioSystem::AudioSystem(std::shared_ptr<Scene> &scene) : SystemI(scene) {}

    void AudioSystem::initial() {
        FMOD::System_Create(&mFmodSystem);
        mFmodSystem->init(32, FMOD_INIT_NORMAL, nullptr);
    }

    void AudioSystem::beforeUpdate() {

    }

    void AudioSystem::afterUpdate() {

    }

    void AudioSystem::onUpdate(TimeDuration dt) {
        manageSound();

        // manage listeners
        ComponentGroup<SPW::IDComponent,SPW::TransformComponent,SPW::AudioListener> ListenerGroup;
        locatedScene.lock()->forEachEntityInGroup(
                ListenerGroup,
                [this](const Entity &en) {
                    auto trans = en.component<TransformComponent>();
                    auto player = en.component<AudioListener>();
                    FMOD_VECTOR listenerPos = { trans->position.x, trans->position.y, trans->position.z };
                    FMOD_VECTOR listenerVel = { 0, 0, 0 };
                    glm::vec4 forward(0, 0, -1, 0);
                    forward = forward * glm::eulerAngleY(glm::radians(trans->rotation.y));
                    FMOD_VECTOR listenerForward = { forward.x, forward.y, -forward.z }; 
                    FMOD_VECTOR listenerUp = { 0, 1, 0 }; 
                    mFmodSystem->set3DListenerAttributes(player->Listener_id, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
                });
        mFmodSystem->update();
    }

    void AudioSystem::onStop() {
        for (auto [oath, sound] : soundData) {
            sound->release();
        }
        mFmodSystem->close();
        mFmodSystem->release();
    }

    FMOD::Channel* AudioSystem::playSound(const std::string &path, bool is3D, bool isLoop) {
        FMOD_MODE mode = is3D ? FMOD_3D : FMOD_2D;
        mode |= isLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
        if (soundData.find(path) == soundData.end()) {
            // lazy load
            FMOD::Sound *newSound = nullptr;
            mFmodSystem->createSound(
                    path.c_str(),
                    mode,
                    nullptr,
                    &newSound);
            soundData[path] = newSound;
        }

        FMOD::Sound *sound = soundData[path];
        if (sound) {
            sound->setMode(mode);
            FMOD::Channel *chan = nullptr;
            mFmodSystem->playSound(sound, nullptr, false, &chan);
            return chan;
        }
        return nullptr;
    }

    void AudioSystem::manageSound() {
        ComponentGroup<SPW::IDComponent, SPW::TransformComponent, SPW::AudioComponent> AudioGroup;
        locatedScene.lock()->forEachEntityInGroup(
                AudioGroup,
                [this](const Entity &en) {
                    auto sc = en.component<AudioComponent>();
                    auto tc = en.component<TransformComponent>();
                    for (auto [path, sound] : sc->allSounds) {
                        if (sound->needUpdate()) {
                            switch (sound->state) {
                                case SoundState::Play:
                                    sound->play(this);
                                    break;
                                case SoundState::Pause:
                                    sound->pause();
                                    break;
                                case SoundState::Continue: {
                                    sound->resume();
                                    break;
                                }
                                case SoundState::Stop:
                                    sound->stop();
                                    break;
                            }
                        }
                        sound->update();
                        sound->setPos(tc->position.x, tc->position.y, tc->position.z);
                    }
                });
    }
}
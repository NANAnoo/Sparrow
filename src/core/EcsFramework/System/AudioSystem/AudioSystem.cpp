#include "AudioSystem.h"
#include "./EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/Scene.hpp"
#include "./EcsFramework/Component/Audio/AudioComponent.h"
#include "./EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/TransformComponent.hpp"
#include <fmod.hpp>
#include "string"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace SPW {

    AudioSystem::AudioSystem(std::shared_ptr<Scene> &scene) : SystemI(scene) {}

    void AudioSystem::initial() {
        auto  res = FMOD::System_Create(&mFmodSystem);
        res = mFmodSystem->init(32, FMOD_INIT_NORMAL, nullptr);
    }

    void AudioSystem::beforeUpdate() {

    }

    void AudioSystem::afterUpdate() {

    }

    void AudioSystem::onUpdate(TimeDuration dt) {
        manageSound();

        // manage listeners
//        ComponentGroup<SPW::IDComponent,SPW::TransformComponent,SPW::AudioListener> ListenerGroup;
//        locatedScene.lock()->forEachEntityInGroup(
//                ListenerGroup,
//                [this,&ListenerGroup](const Entity &en) {
//                    auto trans = en.component<TransformComponent>();
//                    auto player = en.component<AudioListener>();
//                    FMOD_VECTOR listenerPos = { trans->position.x, trans->position.y, trans->position.z }; // 获取当前帧的监听器位置
//                    FMOD_VECTOR listenerVel = { 0, 0, 0 }; // 获取当前帧的监听器速度
//                    FMOD_VECTOR listenerForward = { trans->rotation.x, trans->rotation.y, trans->rotation.z }; // 获取当前帧的监听器前向方向
//                    FMOD_VECTOR listenerUp = { 0, 1, 0 }; // 获取当前帧的监听器上向方向
//
//                    mFmodSystem->set3DListenerAttributes(player->Listener_id, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
//
//                });
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
        FMOD_MODE mode = FMOD_DEFAULT;
        if (is3D) {
            mode = FMOD_3D;
        }
        if (isLoop) {
            mode = FMOD_LOOP_NORMAL;
        }
        if (soundData.find(path) == soundData.end()) {
            // lazy load
            FMOD::Sound *newSound = nullptr;
            auto res = mFmodSystem->createSound(
                    path.c_str(),
                    mode,
                    nullptr,
                    &newSound);
            soundData[path] = newSound;
        }

        FMOD::Sound *sound = soundData[path];
        if (sound) {
            //sound->setMode(mode);
            FMOD::Channel *chan = nullptr;
            auto res = mFmodSystem->playSound(sound, nullptr, false, &chan);
            return chan;
        }
        return nullptr;
    }

    void AudioSystem::manageSound() {

        ComponentGroup<SPW::IDComponent, SPW::TransformComponent, SPW::AudioComponent> AudioGroup;

        locatedScene.lock()->forEachEntityInGroup(
                AudioGroup,
                [this, &AudioGroup](const Entity &en) {
                    auto sc = en.component<AudioComponent>();
                    auto tc = en.component<TransformComponent>();
                    for (auto [path, sound] : sc->allSounds) {
                        if (sound->needUpdate()) {
                            switch (sound->state) {
                                case SoundState::Play:
                                    sound->play(this);
                                case SoundState::Pause:
                                    sound->pause();
                                case SoundState::Continue:
                                    sound->resume();
                                case SoundState::Stop:
                                    sound->stop();
                            }
                        }
                        sound->setPos(tc->position.x, tc->position.y, tc->position.z);
                    }

//                    for (int i = 0; i < sc->audioPaths.size(); i++)
//                    {
//                        FMOD:: Channel *currentChannel = (sc->getChannel(sc->audioPaths[i]));
//                        switch (sc->getState()) {
//                            case Play:
//                                if (sc->is3D) {
//                                    //3D
//                                    if (sc->isLoop) {
//                                        if(sc->getCreate(sc->audioPaths[i]) == false){
//                                            mFmodSystem->createSound(sc->audioPaths[i].c_str(), FMOD_LOOP_NORMAL, 0,&sc->Sound);
//                                            sc->setCreate(sc->audioPaths[i],true);
//                                            mFmodSystem->playSound(sc->Sound, nullptr, false, &currentChannel);
//                                            sc->setChannel(sc->audioPaths[i],currentChannel);
//                                        }
//
//                                    }else{
//                                        if(sc->getCreate(sc->audioPaths[i]) == false){
//                                            mFmodSystem->createSound(sc->audioPaths[i].c_str(), FMOD_3D, 0,&sc->Sound);
//                                            sc->setCreate(sc->audioPaths[i],true);
//                                            mFmodSystem->playSound(sc->Sound, nullptr, false, &currentChannel);
//                                            sc->setChannel(sc->audioPaths[i],currentChannel);
//                                        }
//
//                                    }
//
//                                } else {
//                                    //2D
//                                    if (sc->isLoop) {
//                                        if(currentChannel == nullptr){
//                                            mFmodSystem->createSound(sc->AudioPath.c_str(), FMOD_LOOP_NORMAL, 0,&sc->Sound);
//                                            mFmodSystem->playSound(sc->Sound, nullptr, false, &currentChannel);
//                                            sc->setChannel(sc->audioPaths[i],currentChannel);;
//                                        }
//
//                                    }else{
//                                        if(currentChannel == nullptr){
//                                            mFmodSystem->createSound(sc->AudioPath.c_str(), FMOD_2D, 0, &sc->Sound);
//                                            mFmodSystem->playSound(sc->Sound, nullptr, false, &currentChannel);
//                                            sc->setChannel(sc->audioPaths[i],currentChannel);
//
//                                        }
//                                    }
//                                }
//
//                                break;
//                            case Pause:
//                                currentChannel->setPaused(true);
//
//                                break;
//                            case Continue:
//                                currentChannel->setPaused(false);
//                                printf("sdsds");
//                                break;
//                            case Stop:
//                                currentChannel->stop();
//                                break;
//                        }
//                    }

                });

    }

    void AudioSystem::matchSound() {
        Music uiMusic;

        ComponentGroup<SPW::IDComponent, SPW::TransformComponent, SPW::AudioComponent> AudioGroup;

        locatedScene.lock()->forEachEntityInGroup(
                AudioGroup,
                [this, &uiMusic, &AudioGroup](const Entity &en) {
                    int i;
//                    auto sc = en.component<AudioComponent>();
//                    sc->audioPaths.push_back(sc->AudioPath);
//                    for (i = 0; i < sc->audioPaths.size(); i++) {
//                        FMOD::Channel *Channel = nullptr;
//                        sc->setChannel(sc->audioPaths[i], Channel);
//                        sc->setCreate(sc->audioPaths[i], false);
//
//                    }

                });
    }

}
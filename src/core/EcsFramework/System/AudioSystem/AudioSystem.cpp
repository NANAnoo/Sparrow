#include "AudioSystem.h"
#include "./EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/Scene.hpp"
#include "./EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/TransformComponent.hpp"
#include <fmod.hpp>
#include "string"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace SPW{

    AudioSystem::AudioSystem(std::shared_ptr<Scene> &scene) : SystemI(scene){}

    void AudioSystem::set3DModule(FMOD_MODE f) {
        f = FMOD_3D;
    }

    void AudioSystem::initial() {
        FMOD::System_Create(&mFmodSystem);
        mFmodSystem->init(32,FMOD_INIT_NORMAL,0);

    }

    void AudioSystem::beforeUpdate() {

    }

    void AudioSystem::afterUpdate() {

    }

    void AudioSystem::onUpdate(TimeDuration dt) {

        playSound();
        pausedSound();
    }

    void AudioSystem::onStop() {

        mFmodSystem->close();
        mFmodSystem->release();
    }


    void AudioSystem::playSound() {
        Music uiMusic;

        ComponentGroup<SPW::IDComponent,SPW::TransformComponent,SPW::AudioComponent> AudioGroup;

        locatedScene.lock()->forEachEntityInGroup(
                AudioGroup,
                [this,&uiMusic,&AudioGroup](const Entity &en) {
                    auto sc = en.component<AudioComponent>();

                    if(sc->getTime()<1 && sc->isPlay)
                    {
                        sc->isPaused = false;
                        if(sc->is3D) {
                            //3D
                            mFmodSystem->createSound("./resources/sounds/test.wav", FMOD_3D, 0, &sc->Sound);

                            if(sc->isLoop){
                                mFmodSystem->createSound("./resources/sounds/test.wav", FMOD_LOOP_NORMAL, 0, &sc->Sound);
                            }
                            mFmodSystem->playSound(sc->Sound, nullptr, false, &sc->Channel);
                            sc->addTime();

                        }else{
                            //2D
                            mFmodSystem->createSound("./resources/sounds/test.wav", FMOD_2D, 0, &sc->Sound);
                            if(sc->isLoop){
                                mFmodSystem->createSound("./resources/sounds/test.wav", FMOD_LOOP_NORMAL, 0, &sc->Sound);
                            }
                            mFmodSystem->playSound(sc->Sound, nullptr, false, &sc->Channel);
                            sc->addTime();
                        }

                    }

                });
    }

    void AudioSystem::pausedSound() {
        Music uiMusic;

        ComponentGroup<SPW::IDComponent,SPW::TransformComponent,SPW::AudioComponent> AudioGroup;

        locatedScene.lock()->forEachEntityInGroup(
                AudioGroup,
                [this,&uiMusic,&AudioGroup](const Entity &en) {
                    auto sc = en.component<AudioComponent>();
                    if(sc->isPaused == true)
                    {
                        sc->isPlay = false;
                    }
                    sc->Channel->setPaused(sc->isPaused);
                });
    }

}

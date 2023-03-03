#include "AudioSystem.h"
#include "./EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/Scene.hpp"
#include "./EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/TransformComponent.hpp"
#include <fmod.hpp>
#include "string"


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

        Music uiMusic;

        ComponentGroup<SPW::IDComponent,SPW::TransformComponent,SPW::AudioComponent> AudioGroup;

        std::vector<SPW::Entity> AudioModels;

        locatedScene.lock()->forEachEntityInGroup(
                AudioGroup,
                [this,&uiMusic,&AudioGroup](const Entity &en) {
                    auto sc = en.component<AudioComponent>();
                    if(sc->is3D){
                        set3DModule(sc->ModeType);
                        mFmodSystem->createSound("Sparrow/resources/sounds/test.wav", FMOD_DEFAULT, 0, &sc->Sound);

                        mFmodSystem->playSound(sc->Sound, nullptr, false, &sc->Channel);
                    }else{
                        //TODO
                        mFmodSystem->createSound("Sparrow/resources/sounds/test.wav", FMOD_DEFAULT, 0, &sc->Sound);

                        mFmodSystem->playSound(sc->Sound, nullptr, false, &sc->Channel);
                    }
                });
    }

    void AudioSystem::afterUpdate() {

    }

    void AudioSystem::onUpdate(TimeDuration dt) {
        mFmodSystem->update();
    }

    void AudioSystem::onStop() {
        mFmodSystem->close();

        mFmodSystem->release();
    }
}

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

        Music uiMusic;

        ComponentGroup<SPW::IDComponent,SPW::TransformComponent,SPW::AudioComponent> AudioGroup;

        locatedScene.lock()->forEachEntityInGroup(
                AudioGroup,
                [this,&uiMusic,&AudioGroup](const Entity &en) {
                    auto sc = en.component<AudioComponent>();
                    if(sc->is3D){
                        //3D
                        auto trans = en.component<TransformComponent>();
                        set3DModule(sc->ModeType);
                        FMOD_VECTOR pos = {trans->position.x,trans->position.y,trans->position.z};
                        FMOD_VECTOR vel = {0.0f, 0.0f, 0.0f};
                        sc->Channel->set3DAttributes(&pos,&vel);

                        mFmodSystem->createSound("./resources/sounds/test.wav", FMOD_3D, 0, &sc->Sound);

                        mFmodSystem->playSound(sc->Sound, nullptr, false, &sc->Channel);

                    }else{
                        //2D
                        mFmodSystem->createSound("./resources/sounds/test.wav", FMOD_2D, 0, &sc->Sound);
                        std::cout<<"sds";
                        mFmodSystem->playSound(sc->Sound, nullptr, false, &sc->Channel);
                    }
                });
    }

    void AudioSystem::beforeUpdate() {

    }

    void AudioSystem::afterUpdate() {

    }

    void AudioSystem::onUpdate(TimeDuration dt) {

        Music uiMusic;

        ComponentGroup<SPW::IDComponent,SPW::TransformComponent,SPW::AudioComponent> AudioGroup;

        locatedScene.lock()->forEachEntityInGroup(
                AudioGroup,
                [this,&uiMusic,&AudioGroup](const Entity &en){
                    auto sc = en.component<AudioComponent>();
                    auto transform = en.component<TransformComponent>();

                    glm::vec3 old_pos = en.component<SPW::TransformComponent>()->position;
                    glm::mat4 trans = glm::mat4(1.0f);
                    trans = glm::translate(trans, glm::vec3(0.005f, -0.0005f, 0.0f));


                    en.component<SPW::TransformComponent>()->position = trans * glm::vec4(old_pos,1.0f);

                    FMOD_VECTOR pos = {en.component<SPW::TransformComponent>()->position.x,en.component<SPW::TransformComponent>()->position.y,en.component<SPW::TransformComponent>()->position.z};

                    std::cout<<en.component<SPW::TransformComponent>()->position.x<< " " << en.component<SPW::TransformComponent>()->position.y << " " <<
                                                                                                                                                       en.component<SPW::TransformComponent>()->position.z << std::endl;
                    FMOD_VECTOR vel = {0.0f, 0.0f, 0.0f};
                    sc->Channel->set3DAttributes(&pos,&vel);


                });

        mFmodSystem->update();
    }

    void AudioSystem::onStop() {
        mFmodSystem->close();

        mFmodSystem->release();
    }
}

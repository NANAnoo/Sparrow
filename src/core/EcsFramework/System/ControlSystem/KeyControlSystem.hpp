//
// Created by 29134 on 2023/2/25.
//
#pragma once
#include "Control/KeyEvent.hpp"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/System/SystemI.h"
#include "EcsFramework/Component/KeyComponent.hpp"

namespace SPW {
    class KeyControlSystem :public SPW::KeyEventResponder, public SPW::SystemI{
    public:

        KeyControlSystem(std::shared_ptr<Scene> &scene)
                : KeyEventResponder(scene), SystemI(scene) {};

        bool onKeyDown(SPW::KeyEvent *e) final{

            key_queue.push(e);

            return false;
        }

        bool onKeyHeld(SPW::KeyEvent *e) final{

            key_queue.push(e);

            return false;
        }

        bool onKeyReleased(SPW::KeyEvent *e) final{

            key_queue.push(e);

            return false;
        }

        void initial() final{}
        void beforeUpdate() final{

            while(key_queue.size() != 0){

                auto e = key_queue.front();

                switch (e->type()) {

                    case SPW::EventType::KeyDownType:
                        locatedScene.lock()->forEachEntity<SPW::KeyComponent>([&e](const Entity &entity){
                        if(entity.component<KeyComponent>()->onKeyDownCallBack)
                            entity.component<KeyComponent>()->onKeyDownCallBack(entity, e->keycode);
                    });
                        break;

                    case SPW::EventType::KeyHeldType:
                        locatedScene.lock()->forEachEntity<SPW::KeyComponent>([&e](const Entity &entity){
                        if(entity.component<KeyComponent>()->onKeyHeldCallBack)
                            entity.component<KeyComponent>()->onKeyHeldCallBack(entity, e->keycode);
                    });
                        break;

                    case SPW::EventType::KeyReleasedType:
                        locatedScene.lock()->forEachEntity<SPW::KeyComponent>([&e](const Entity &entity){
                        if(entity.component<KeyComponent>()->onKeyReleasedCallBack)
                            entity.component<KeyComponent>()->onKeyReleasedCallBack(entity, e->keycode);
                    });
                        break;
                    default:
                    break;
                }
                key_queue.pop();
            }
        }
        
        void onUpdate(TimeDuration dt) final{}
        void afterUpdate() final{}
        void onStop() final{}

    private:
        std::queue<KeyEvent *> key_queue;
    };
}

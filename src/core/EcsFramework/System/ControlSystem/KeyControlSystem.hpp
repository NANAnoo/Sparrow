//
// Created by 29134 on 2023/2/25.
//
#pragma once
#include "Control/KeyEvent.hpp"
#include "EcsFramework/System/SystemI.h"

namespace SPW {
    class KeyControlSystem :public SPW::KeyEventResponder, public SPW::SystemI{
    public:

        KeyControlSystem(std::shared_ptr<Scene> &scene)
                : KeyEventResponder(scene), SystemI(scene) {};

        bool onKeyDown(SPW::KeyEvent *e) final{

            locatedScene.lock()->forEachEntity<SPW::KeyComponent>([&e](const Entity &entity){
                if(entity.component<KeyComponent>()->onKeyDownCallBack)
                    entity.component<KeyComponent>()->onKeyDownCallBack(entity, e->keycode);
            });

            return false;
        }

        bool onKeyHeld(SPW::KeyEvent *e) final{

            locatedScene.lock()->forEachEntity<SPW::KeyComponent>([&e](const Entity &entity){
                if(entity.component<KeyComponent>()->onKeyHeldCallBack)
                    entity.component<KeyComponent>()->onKeyHeldCallBack(entity, e->keycode);
            });

            return false;
        }

        bool onKeyReleased(SPW::KeyEvent *e) final{

            locatedScene.lock()->forEachEntity<SPW::KeyComponent>([&e](const Entity &entity){
                if(entity.component<KeyComponent>()->onKeyReleasedCallBack)
                    entity.component<KeyComponent>()->onKeyReleasedCallBack(entity, e->keycode);
            });

            return false;
        }

        void initial() final{}
        void beforeUpdate() final{}
        void onUpdate(TimeDuration dt) final{}
        void afterUpdate() final{}
        void onStop() final{}

    };
}

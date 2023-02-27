//
// Created by 29134 on 2023/2/25.
//
#pragma once
#include "Control/KeyEvent.hpp"
#include "Control/KeyCodes.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/System/SystemI.h"
#include "EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/Scene.hpp"

namespace SPW {
    class KeyControlSystem :public SPW::KeyEventResponder, public SPW::SystemI{
    public:

        KeyControlSystem(std::shared_ptr<Scene> &scene)
                : KeyEventResponder(scene), SystemI(scene) {};

        bool onKeyDown(SPW::KeyEvent *e) override final{

            locatedScene.lock()->forEach([](SPW::KeyComponent * keyC) {

            } , SPW::KeyComponent);
            return true;
        }

        bool onKeyHeld(SPW::KeyEvent *e) override final{

            return false;
        }

        bool onKeyReleased(SPW::KeyEvent *e) override final{

            return false;
        }

        void initial() final;
        void beforeUpdate() final{}
        void onUpdate(TimeDuration dt) final;
        void afterUpdate() final;
        void onStop() final;

    };
}

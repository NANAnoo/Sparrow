//
// Created by 29134 on 2023/2/26.
//
#pragma once
#include "Control/MouseEvent.hpp"
#include "EcsFramework/System/SystemI.h"

namespace SPW {
    class MouseControlSystem : public SPW::MouseEventResponder, public SPW::SystemI{
    public:

        MouseControlSystem(std::shared_ptr<Scene> &scene): MouseEventResponder(scene), SystemI(scene) {};

        bool onMouseDown(SPW::MouseEvent *e) override{

            locatedScene.lock()->forEachEntity<MouseComponent>([&e](const Entity &entity){
                if(entity.component<MouseComponent>()->onMouseDownCallBack)
                    entity.component<MouseComponent>()->onMouseDownCallBack(entity, e->button_code);
            });

            return false;
        }

        bool onMouseHeld(SPW::MouseEvent *e) {

            locatedScene.lock()->forEachEntity<MouseComponent>([&e](const Entity &entity){
                if(entity.component<MouseComponent>()->onMouseHeldCallBack)
                    entity.component<MouseComponent>()->onMouseHeldCallBack(entity, e->button_code);
            });

            return false;
        };

        bool onMouseReleased(MouseEvent *e) {

            locatedScene.lock()->forEachEntity<MouseComponent>([&e](const Entity &entity){
                if(entity.component<MouseComponent>()->onMouseReleasedCallBack)
                    entity.component<MouseComponent>()->onMouseReleasedCallBack(entity, e->button_code);
            });

            return false;
        };

        bool cursorMovement(MouseEvent *e) {

            return false;
        };

        bool onMouseScroll(MouseEvent *e) {

            locatedScene.lock()->forEachEntity<MouseComponent>([&e](const Entity &entity){
                if(entity.component<MouseComponent>()->onMouseScrollCallBack)
                    entity.component<MouseComponent>()->onMouseScrollCallBack(entity, e->scroll_offset);
            });

            return false;
        };

        void initial(){};
        void beforeUpdate(){};
        void onUpdate(TimeDuration dt){};
        void afterUpdate(){};
        void onStop(){};
    };
}
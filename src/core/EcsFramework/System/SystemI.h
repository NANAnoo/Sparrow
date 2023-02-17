//
// Created by ZhangHao on 2023/2/15.
//

#ifndef SPARROW_SYSTEMI_H
#define SPARROW_SYSTEMI_H

#include <memory>
#include "Utils/Timer.hpp"

namespace SPW {
    class Scene;
    class SystemI {
    public:
        explicit SystemI(std::shared_ptr<Scene> &scene) :
            locatedScene(scene) {

        }
        virtual void initial() = 0;
        virtual void beforeUpdate() = 0;
        virtual void onUpdate(TimeDuration dt) = 0;
        virtual void afterUpdate() = 0;
        virtual void onStop() = 0;

    private:
        std::weak_ptr<Scene> locatedScene;
    };
}

#endif //SPARROW_SYSTEMI_H

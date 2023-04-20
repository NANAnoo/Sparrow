//
// Created by ZhangHao on 2023/2/15.
//

#ifndef SPARROW_SYSTEMI_H
#define SPARROW_SYSTEMI_H

#include <memory>
#include "Utils/Timer.hpp"
#include "queue"
#include <iostream>

namespace SPW {
    class Scene;
    class SystemI {
    public:
        explicit SystemI(std::shared_ptr<Scene> &scene) :
            locatedScene(scene) {

        }

        virtual void setPausd(bool pause)
    	{
            isPaused = pause;
        }

    	virtual void initial() = 0;
        virtual void beforeUpdate() = 0;
        virtual void onUpdate(TimeDuration dt) = 0;
        virtual void afterUpdate() = 0;
        virtual void onStop() = 0;
        


    protected:
        std::weak_ptr<Scene> locatedScene;
        bool isPaused = false;
    };
}

#endif //SPARROW_SYSTEMI_H

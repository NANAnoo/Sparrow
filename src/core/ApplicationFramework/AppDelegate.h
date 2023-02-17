//
// Created by NANAnoo on 2/9/2023.
//

#ifndef SPARROW_APPLICATION_H
#define SPARROW_APPLICATION_H

#include <vector>
#include "Utils/Timer.hpp"
#include "Event/Event.h"

namespace SPW {
    class Application;
    class EventI;
    // application delegate interface
    class AppDelegateI : public EventResponderI {
    public:
        explicit AppDelegateI(std::shared_ptr<EventResponderI> &app) :
            EventResponderI(app){
        }
// life cycle of application
<<<<<<< HEAD
        virtual void onAppInit(std::shared_ptr<Application> app) = 0;
        virtual void beforeAppUpdate(std::shared_ptr<Application> app) = 0;
        virtual void onAppUpdate(std::shared_ptr<Application> app, const TimeDuration &) = 0;
        virtual void afterAppUpdate(std::shared_ptr<Application> app) = 0;
        virtual void onUnConsumedEvents(std::shared_ptr<Application> app, std::vector<std::shared_ptr<EventI>> &) = 0;
        virtual void onAppStopped(std::shared_ptr<Application> app) = 0;
=======
        virtual void onAppInit() = 0;
        virtual void beforeAppUpdate() = 0;
        virtual void onAppUpdate(const TimeDuration &) = 0;
        virtual void afterAppUpdate() = 0;
        virtual void onUnConsumedEvents(std::vector<std::shared_ptr<EventI>> &) = 0;
        virtual void onAppStopped() = 0;
>>>>>>> main
        std::shared_ptr<Application> app;
    };
};

#endif //SPARROW_APPLICATION_H

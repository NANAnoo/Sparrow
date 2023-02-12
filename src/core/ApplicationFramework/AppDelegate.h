//
// Created by NANAnoo on 2/9/2023.
//

#ifndef SPARROW_APPLICATION_H
#define SPARROW_APPLICATION_H

#include <vector>
#include "Utils/Timer.hpp"

namespace SPW {
    class Application;
    class EventI;
    // application delegate interface
    class AppDelegateI {
    public:
        // life cycle of application
        virtual void onAppInit(Application &app) = 0;
        virtual void beforeAppUpdate(const Application &app) = 0;
        virtual void onAppUpdate(const Application &app, const TimeDuration &) = 0;
        virtual void afterAppUpdate(const Application &app) = 0;
        virtual void onEvent(const Application &app, const EventI &e) = 0;
        virtual void onUnConsumedEvents(const Application &app, std::vector<EventI> &) = 0;
        virtual void onAppStopped(const Application &app) = 0;
        virtual void onAppDestroy() = 0;
    };
};

#endif //SPARROW_APPLICATION_H

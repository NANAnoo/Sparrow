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
        virtual void onAppInit(std::shared_ptr<Application> app) = 0;
        virtual void beforeAppUpdate(std::shared_ptr<Application> app) = 0;
        virtual void onAppUpdate(std::shared_ptr<Application> app, const TimeDuration &) = 0;
        virtual void afterAppUpdate(std::shared_ptr<Application> app) = 0;
        virtual void onUnConsumedEvents(std::shared_ptr<Application> app, std::vector<std::shared_ptr<EventI>> &) = 0;
        virtual void onAppStopped(std::shared_ptr<Application> app) = 0;
        virtual void onAppDestroy() = 0;
    };
};

#endif //SPARROW_APPLICATION_H

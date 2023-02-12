#include "Application.h"
#include <iostream>
#include "Utils/MessageDefines.h"
#include "Utils/Timer.hpp"

#define LIFE_CYCLE(stage)                       \
do {                                            \
    for (auto &wk_delegate : delegates) {       \
        if (!wk_delegate.expired()) {           \
            auto strong = wk_delegate.lock();   \
            strong->stage;                      \
        }                                       \
    }                                           \
 }while(0)

SPW::Application::Application() {
    POST_MSG(SPW::kMsgApplicationCreated)
}

SPW::Application::~Application() {
    stop();
    // notify delegates
    LIFE_CYCLE(onAppDestroy());
}

int SPW::Application::run(int argc, char **argv) {
    // update arguments
    for (int i = 0; i < argc; i ++) {
        arguments.emplace_back(argv[i]);
    }
    // init app
    init();
    // record time gap
    // main loop of application
    static SPW::TimeStamp begin{};
    while (isRunning) {
        // calculate duration
        TimeDuration du = SPW::Timer::current() - begin;
        // update time stamp
        begin = SPW::Timer::current();

        // one life cycle
        // before update
        POST_MSG(SPW::kMsgBeforeAppUpdate);
        LIFE_CYCLE(beforeAppUpdate(*this));

        // on update
        LIFE_CYCLE(onAppUpdate(*this, du));
        window->onUpdate();

        // after update
        POST_MSG(SPW::kMsgAfterAppUpdate);
        LIFE_CYCLE(afterAppUpdate(*this));
        // pull events
        LIFE_CYCLE(onUnConsumedEvents(*this, unhandledEvents));
    }
    return 0;
}

void SPW::Application::stop() {
    isRunning = false;
    LIFE_CYCLE(onAppStopped(*this));
    POST_MSG(SPW::kMsgApplicationStopped);
}

void SPW::Application::addDelegate(const std::weak_ptr<AppDelegateI> &delegate) {
    delegates.emplace_back(delegate);
}

void SPW::Application::init() {
    isRunning = true;
    EventI::EventHanlder handler = [this](const EventI &e){
        LIFE_CYCLE(onEvent(*this, e));
        if (!e.consumed) {
            unhandledEvents.emplace_back(e);
        }
    };
    LIFE_CYCLE(onAppInit(*this));
    window->init(WindowMeta({window->title(), window->width(), window->height(), handler}));
    POST_MSG(SPW::kMsgApplicationInited)
}

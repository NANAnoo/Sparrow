#include "Application.h"
#include <iostream>
#include "Utils/MessageDefines.h"

#define LIFE_CYCLE(stage)                       \
do {                                            \
    for (auto &wk_delegate : delegates) {       \
        if (!wk_delegate.expired()) {           \
            auto strong = wk_delegate.lock();   \
            strong->stage;                      \
        }                                       \
    }                                           \
 }while(0)

SPW::Application::Application(const char *name) {
    _name = name;
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
    // main loop of application
    while (isRunning) {
        POST_MSG(SPW::kMsgBeforeAppUpdate);
        LIFE_CYCLE(beforeAppUpdate(*this));
        LIFE_CYCLE(onAppUpdate(*this));
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
    Event::EventHanlder handler = [this](const Event &e){
        LIFE_CYCLE(onEvent(*this, e));
        if (!e.isConsumed) {
            unhandledEvents.emplace_back(e);
        }
    };
    window->init(WindowMeta({"Sparrow", 1600, 900, handler}));
    LIFE_CYCLE(onAppInit(*this));
    POST_MSG(SPW::kMsgApplicationInited)
}

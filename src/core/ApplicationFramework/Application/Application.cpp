#include "Application.h"

#include "Utils/MessageDefines.h"
#include "Utils/Timer.hpp"

SPW::Application::Application(const std::shared_ptr<AppDelegateI>& delegate)
        :EventResponderI() {

    this->delegate = delegate;
    POST_MSG(SPW::kMsgApplicationCreated)
}

SPW::Application::~Application() {
    stop();
    // notify delegate
    delegate.lock()->onAppDestroy();
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
        POST_MSG(SPW::kMsgBeforeAppUpdate)
        delegate.lock()->beforeAppUpdate(weakThis.lock());

        // on update
        delegate.lock()->onAppUpdate(weakThis.lock(), du);
        window->onUpdate();

        // after update
        POST_MSG(SPW::kMsgAfterAppUpdate)
        delegate.lock()->afterAppUpdate(weakThis.lock());
        // pull events
        delegate.lock()->onUnConsumedEvents(weakThis.lock(), unhandledEvents);
    }
    return 0;
}

void SPW::Application::stop() {
    if (!isRunning) return;
    isRunning = false;
    delegate.lock()->onAppStopped(weakThis.lock());
    POST_MSG(SPW::kMsgApplicationStopped)
}

void SPW::Application::init() {
    isRunning = true;
    EventI::EventHanlder handler = [this] (EventI &e){
        onEvent(&e);
        if (!e.consumed) {
            unhandledEvents.emplace_back(e);
        }
    };
    delegate.lock()->onAppInit(weakThis.lock());
    window->init(WindowMeta({window->title(), window->width(), window->height(), handler}));
    POST_MSG(SPW::kMsgApplicationInited)
}

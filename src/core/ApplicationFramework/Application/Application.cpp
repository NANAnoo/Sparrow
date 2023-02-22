#include "Application.h"

#include "Utils/MessageDefines.h"
#include "Utils/Timer.hpp"

SPW::Application::Application()
        :EventResponderI() {
    POST_MSG(SPW::kMsgApplicationCreated)
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
        // pull events, update window
        window->getGraphicsContext()->SwapBuffers();
        window->onUpdate();
        // calculate duration
        TimeDuration du = SPW::Timer::current() - begin;
        // update time stamp
        begin = SPW::Timer::current();

        // one life cycle
        // before update
        POST_MSG(SPW::kMsgBeforeAppUpdate)
        delegate.lock()->beforeAppUpdate();

        // on update
        delegate.lock()->onAppUpdate(du);

        // after update
        POST_MSG(SPW::kMsgAfterAppUpdate)
        delegate.lock()->afterAppUpdate();

        delegate.lock()->onUnConsumedEvents(unhandledEvents);
        unhandledEvents.clear();
    }
    return 0;
}

void SPW::Application::stop() {
    if (!isRunning) return;
    isRunning = false;
    delegate.lock()->onAppStopped();
    POST_MSG(SPW::kMsgApplicationStopped)
}

void SPW::Application::init() {
    isRunning = true;
    EventI::EventHanlder handler = [this] (const std::shared_ptr<EventI> &e){
        onEvent(e);
        if (!e->consumed) {
            unhandledEvents.emplace_back(e);
        }
    };
    delegate.lock()->onAppInit();
    window->init(WindowMeta({window->title(), window->width(), window->height(), handler}));
    POST_MSG(SPW::kMsgApplicationInited)
}

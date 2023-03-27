#pragma once

#include "ApplicationFramework/AppDelegate.h"
#include "ApplicationFramework/WindowI/WindowI.h"

#include <vector>
#include <memory>

namespace SPW {
    // root node of event responder
    class Application : public EventResponderI {
    public:
        explicit Application();
        // run with an exit code
        int run(int argc = 0, char **argv = nullptr);
        // stop
        void stop();
        // register delegate
        // get arguments
        std::vector<std::string> &getArgumentList() {return arguments;}
        // window
        std::shared_ptr<WindowI> window = nullptr;
        // post event
        void postEvent(const std::shared_ptr<EventI> &e) {
            onEvent(e);
        }

        // event
        const char *getName() override {return "Application";}

        // init app with create
        template<class T, typename ... Args>
        static std::shared_ptr<AppDelegateI> create(Args ... args) {
            auto app = std::make_shared<Application>();
            auto ptr = std::shared_ptr<EventResponderI>(app);
            auto delegate = std::shared_ptr<AppDelegateI>(new T(ptr, std::forward<Args>(args)...));
            app->delegate = delegate;
            delegate->app = app;
            return delegate;
        }
        std::weak_ptr<AppDelegateI> delegate;
    private:
        void init();
        std::vector<std::string> arguments;
        std::vector<std::shared_ptr<EventI>> unhandledEvents;
        bool isRunning = false;
    };

}


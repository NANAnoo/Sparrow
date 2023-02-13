#pragma once

#include "ApplicationFramework/AppDelegate.h"
#include "ApplicationFramework/WindowI/WindowI.h"


#include <vector>
#include <memory>

namespace SPW {
    // root node of event responder
    class Application : public EventResponderI {
    public:
        Application() = delete;
        explicit Application(const std::shared_ptr<AppDelegateI>& delegate);
        ~Application();

        // run with an exit code
        int run(int argc = 0, char **argv = nullptr);
        // stop
        void stop();
        // register delegate
        // get arguments
        std::vector<std::string> &getArgumentList() {return arguments;}
        // window
        std::shared_ptr<WindowI> window = nullptr;

        // weak this
        std::weak_ptr<Application> weakThis;

        // event
        const char *getName() override {return "Application";}

        // init app with create
        static std::shared_ptr<Application> create(std::shared_ptr<AppDelegateI> &delegate) {
            auto app = std::make_shared<Application>(delegate);
            app->weakThis = app;
            return app;
        }
    private:
        void init();
        std::weak_ptr<AppDelegateI> delegate;
        std::vector<std::string> arguments;
        std::vector<EventI> unhandledEvents;
        bool isRunning = false;
    };

}


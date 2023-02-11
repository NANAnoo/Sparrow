#pragma once

#include "ApplicationFramework/AppDelegate.h"
#include "ApplicationFramework/WindowI/WindowI.h"

#include <vector>
#include <memory>

namespace SPW {
    class Application {
    public:
        const char *_name;
        explicit Application(const char *name);
        ~Application();

        // run with an exit code
        int run(int argc = 0, char **argv = nullptr);
        // stop
        void stop();
        // register delegate
        void addDelegate(const std::weak_ptr<AppDelegateI> &delegate);
        // get arguments
        std::vector<std::string> &getArgumentList() {return arguments;}

    private:
        void init();
        std::vector<std::weak_ptr<AppDelegateI>> delegates;
        std::vector<std::string> arguments;
        std::vector<Event> unhandledEvents;
        std::shared_ptr<WindowI> window = nullptr;
        bool isRunning = false;
    };
}


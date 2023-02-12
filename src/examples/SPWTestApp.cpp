#include <iostream>
#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"

class TestDelegate : public SPW::AppDelegateI {

    void onAppInit(SPW::Application &app) final {
        app.window = std::make_shared<SPW::GlfwWindow>();
        app.window->setSize(1920, 1080);
        app.window->setTitle("SPWTestApp");
    }
    void beforeAppUpdate(const SPW::Application &app) final{

    }
    void onAppUpdate(const SPW::Application &app, const SPW::TimeDuration &) final{

    }
    void afterAppUpdate(const SPW::Application &app) final{

    }
    void onEvent(const SPW::Application &app, const SPW::Event &e) final {

    }
    void onUnConsumedEvents(const SPW::Application &app, std::vector<SPW::Event> &) final{

    }
    void onAppStopped(const SPW::Application &app) final{

    }
    void onAppDestroy() final{

    }
};

int main(int argc, char **argv) {
    SPW::Application app("hello");
    auto delegate = std::shared_ptr<SPW::AppDelegateI>(new TestDelegate());
    app.addDelegate(delegate);
    return app.run(argc, argv);
}
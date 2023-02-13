#include <iostream>
#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include "Utils/MessageDefines.h"

#include "Control/MouseEvent.hpp"
#include "Control/KeyEvent.hpp"
#include "ApplicationFramework/WindowI/WindowEvent.h"

// event test class
class wtf : public SPW::KeyEventResponder,
        public SPW::MouthEventResponder,
        public SPW::WindowEventResponder{
public:
    explicit wtf(std::shared_ptr<SPW::Application> &app) :
            SPW::MouthEventResponder(app),
            SPW::KeyEventResponder(app),
            SPW::WindowEventResponder(app){
    }

    bool onMouthDown(SPW::MouthEvent *e) override {
        std::cout << "onMouthDown" << std::endl;
        return false;
    }

    bool onKeyDown(SPW::KeyEvent *e) override {
        std::cout << "onKeyDown" << std::endl;
        return true;
    }

    bool onWindowResize(int w, int h) override {
        std::cout << "Window resize" << "(" << w << " ," << h <<")" <<std::endl;
        return true;
    }

    const char *getName() override {
        return "wtf";
    }
};

class nima : public SPW::KeyEventResponder, public SPW::MouthEventResponder{
public:
    explicit nima(std::shared_ptr<wtf> &parent) :
     SPW::KeyEventResponder(std::shared_ptr<SPW::KeyEventResponder> (parent)),
     SPW::MouthEventResponder(std::shared_ptr<SPW::MouthEventResponder> (parent))
    {
    }

    bool onMouthDown(SPW::MouthEvent *e) override {
        std::cout << "onMouthDown" << std::endl;
        return false;
    }

    bool onKeyDown(SPW::KeyEvent *e) override {
        std::cout << "onKeyDown" << std::endl;
        return true;
    }

    const char *getName() override {
        return "nima";
    }
};

class woc : public SPW::MouthEventResponder{
public:
    explicit woc(std::shared_ptr<nima> &parent):
    SPW::MouthEventResponder(std::shared_ptr<SPW::MouthEventResponder> (parent)) {

    }
    bool onMouthDown(SPW::MouthEvent *e) override {
        std::cout << "onMouthDown" << std::endl;
        return true;
    }
    const char *getName() override {
        return "woc";
    }
};

class TestDelegate : public SPW::AppDelegateI {
    void onAppInit(std::shared_ptr<SPW::Application> app) final {
        app->window = std::make_shared<SPW::GlfwWindow>();
        app->window->setSize(1920, 1080);
        app->window->setTitle("SPWTestApp");

        // event test
        auto test = std::make_shared<wtf>(app);
        auto nim = std::make_shared<nima>(test);
        auto wc = std::make_shared<woc>(nim);
        SPW::MouthEvent me;
        SPW::KeyEvent ke;
        SPW::WindowEvent we(SPW::WindowResizeType, app->window->width(), app->window->height());
        app->onEvent(&me);
        app->onEvent(&ke);
        app->onEvent(&we);
        DEBUG_EXPRESSION(
                for (auto &name:ke.processChain) {
                    std::cout << name << "->";
                }
                std::cout << (ke.consumed ? "[consumed]" : "[not consumed]") << std::endl;

                for (auto &name:me.processChain) {
                    std::cout << name << "->";
                }
                std::cout << (me.consumed ? "[consumed]" : "[not consumed]") << std::endl;
                )

    }
    void beforeAppUpdate(std::shared_ptr<SPW::Application> app) final{

    }
    void onAppUpdate(std::shared_ptr<SPW::Application> app, const SPW::TimeDuration &du) final{
        std::cout << du.toMs() << " ms" << std::endl;
    }

    void afterAppUpdate(std::shared_ptr<SPW::Application> app) final{
        static int count = 0;
        if (count ++ > 10) {
            app->stop();
        }
    }
    void onUnConsumedEvents(std::shared_ptr<SPW::Application> app, std::vector<SPW::EventI> &) final{

    }
    void onAppStopped(std::shared_ptr<SPW::Application> app) final{
        std::cout << "app stopped" << std::endl;
    }
    void onAppDestroy() final{
        std::cout << "app destroyed" << std::endl;
    }
};

int main(int argc, char **argv) {
    // message center test
    SPW::REGISTER_MSG(SPW::kMsgBeforeAppUpdate, [](SPW::Message msg){
        std::cout << "----------------------" << std::endl;
        std::cout << "what" << std::endl;
    })
    SPW::REGISTER_MSG(SPW::kMsgAfterAppUpdate, [](SPW::Message msg){
        std::cout << "fuck" << std::endl;
    })
    SPW::OBSERVE_MSG_ONCE(SPW::kMsgAfterAppUpdate, [](SPW::Message msg){
        std::cout << "only once" << std::endl;
    })

    // app test
    auto delegate = std::shared_ptr<SPW::AppDelegateI>(new TestDelegate());
    auto app = SPW::Application::create(delegate);
    return app->run(argc, argv);

}
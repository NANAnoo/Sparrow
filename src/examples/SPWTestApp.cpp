#include <iostream>
#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include "Utils/MessageDefines.h"

class TestDelegate : public SPW::AppDelegateI {

    void onAppInit(SPW::Application &app) final {
        app.window = std::make_shared<SPW::GlfwWindow>();
        app.window->setSize(1920, 1080);
        app.window->setTitle("SPWTestApp");
    }
    void beforeAppUpdate(const SPW::Application &app) final{

    }
    void onAppUpdate(const SPW::Application &app, const SPW::TimeDuration &du) final{
        std::cout << du.toSecond() << std::endl;
    }
    void afterAppUpdate(const SPW::Application &app) final{

    }
    void onEvent(const SPW::Application &app, const SPW::EventI &e) final {

    }
    void onUnConsumedEvents(const SPW::Application &app, std::vector<SPW::EventI> &) final{

    }
    void onAppStopped(const SPW::Application &app) final{

    }
    void onAppDestroy() final{

    }
};


class base {
public:
    base() {
        std::cout << "base" << std::endl;
    };
    virtual void test() = 0;
    virtual ~base() {
        std::cout << "~base" << std::endl;
    };
};

class son : public base{
public:
    son() {
        std::cout << "son" << std::endl;
    }
    void test() final {
        why();
    }
    virtual void why() = 0;
    virtual ~son() {
        std::cout << "~son" << std::endl;
    }
};

class father : public son{
public:
    father() {
        std::cout << "father" << std::endl;
    }
    void why() override {
        std::cout << "why" << std::endl;
    }
    virtual ~father() {
        std::cout << "~father" << std::endl;
    }
};
int main(int argc, char **argv) {
    {
        std::vector<base *> b;
        father a;
        b.emplace_back(&a);
        b.front()->test();
    }
//    SPW::Application app;
//    SPW::REGISTER_MSG(SPW::kMsgBeforeAppUpdate, [](SPW::Message msg){
//        std::cout << "what" << std::endl;
//    })
//    SPW::REGISTER_MSG(SPW::kMsgAfterAppUpdate, [](SPW::Message msg){
//        std::cout << "fuck \n" << std::endl;
//    })
//    SPW::OBSERVE_MSG_ONCE(SPW::kMsgAfterAppUpdate, [](SPW::Message msg){
//        std::cout << "only once" << std::endl;
//    })
//    auto delegate = std::shared_ptr<SPW::AppDelegateI>(new TestDelegate());
//    app.addDelegate(delegate);
//
//    return app.run(argc, argv);
}
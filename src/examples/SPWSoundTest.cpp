//
// Created by NANAnoo on 2/18/2023.
//

#include <iostream>

#include <sol/sol.hpp>

#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include "Utils/MessageDefines.h"

#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Control/KeyEvent.hpp"
#include "Control/MouseEvent.hpp"

#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Entity/Entity.hpp"

#include "EcsFramework/Component/BasicComponent/NameComponent.h"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "Utils/UUID.hpp"

#include "SimpleRender.h"

#include <fmod.hpp>

class TestDelegate : public SPW::AppDelegateI {
public:
    explicit TestDelegate(std::shared_ptr<SPW::EventResponderI> &app, const char *name) :
            SPW::AppDelegateI(app), _name(name) {
    }
    void onAppInit() final {
        app->window = std::make_shared<SPW::GlfwWindow>();
        app->window->setSize(800, 600);
        app->window->setTitle("SPWTestApp");
        SPW::OBSERVE_MSG_ONCE(SPW::kMsgApplicationInited, [this](SPW::Message msg) {
            this->render = std::make_shared<SimpleRender>();
        })

        if (FMOD_OK != FMOD::System_Create(&system, FMOD_VERSION)) {
            std::cout << "System create failed" <<std::endl;
        }
        if (FMOD_OK != system->init(32, FMOD_INIT_NORMAL, nullptr)) {
            std::cout << "System init failed" <<std::endl;
        }
        if (FMOD_OK != system->createSound("./resources/sounds/test.wav", FMOD_DEFAULT, nullptr, &sound)) {
            std::cout << "Sound create failed" <<std::endl;
        }
        if (FMOD_OK != system->playSound(sound, nullptr, false, &channel)) {
            std::cout << "Sound play failed" <<std::endl;
        }
    }
    void beforeAppUpdate() final{

    }
    void onAppUpdate(const SPW::TimeDuration &du) final{
        // physics, computation
        bool isPlaying;
        channel->isPlaying(&isPlaying);
        system->update();
        if (!isPlaying) {
            app->stop();
        }
    }

    void afterAppUpdate() final{
        render->render();
    }
    void onUnConsumedEvents(std::vector<std::shared_ptr<SPW::EventI>> &events) final{
        for (auto &e : events) {
            DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
        }
    }
    void onAppStopped() final{
        sound->release();
        system->close();
        system->release();

        std::cout << "app stopped" << std::endl;
    }

    void solveEvent(const std::shared_ptr<SPW::EventI> &e) final {
        e->dispatch<SPW::WindowCloseType, SPW::WindowEvent>(
                [this](SPW::WindowEvent *e){
                    // close application
                    app->stop();
                    return true;
                });
        SPW::EventResponderI::solveEvent(e);
    }

    const char *getName() final {return _name;}
    const char *_name;
    std::shared_ptr<SimpleRender> render;
    std::shared_ptr<SPW::Scene> scene;
    FMOD::System *system;
    FMOD::Sound *sound;
    FMOD::Channel *channel;
};

// main entrance
int main(int argc, char **argv) {
    // app test
    auto appProxy =
            SPW::Application::create<TestDelegate>("SPWTestApp");
    return appProxy->app->run(argc, argv);
}
//
// Created by NANAnoo on 3/20/2023.
//

#include <iostream>

#include <sol/sol.hpp>

#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Event/Event.h"

#include "EcsFramework/Scene.hpp"
#include "Platforms/OPENGL/OpenGLxGLFWContext.hpp"

class GameWrapper : public SPW::AppDelegateI {
public:
    explicit GameWrapper(std::shared_ptr<SPW::EventResponderI> &app) :
            SPW::AppDelegateI(app) {
    }
    void onAppInit() final {
        auto window = std::make_shared<SPW::GlfwWindow>();
        app->window = window;

        m_state.open_libraries(sol::lib::base, sol::lib::package);
        std::string x = m_state["package"]["path"];
        m_state["package"]["path"] = x + ";./LuaLib/?.lua;./resources/scripts/lua/?.lua";
        try {
            if(m_state.script_file("./resources/scripts/lua/TestGame.lua").valid()) {
                auto global_app = m_state["global"]["app"];
                onInit = global_app["onInit"];
                beforeUpdate = global_app["beforeUpdate"];
                onUpdate = global_app["onUpdate"];
                afterUpdate = global_app["afterUpdate"];
                onStop = global_app["onStop"];
                std::string name = global_app["name"];
                int width = global_app["width"];
                int height = global_app["height"];
                app->window->setTitle(name.c_str());
                app->window->setSize(width, height);
            }
        } catch (sol::error &e) {
            std::cout << e.what() << std::endl;
        }
        std::weak_ptr<SPW::GlfwWindow> weak_window = window;
        window->onWindowCreated([this, weak_window](GLFWwindow *handle){
            if (weak_window.expired()) {
                return;
            }
            // create graphics context
            weak_window.lock()->graphicsContext = std::make_shared<SPW::OpenGLxGLFWContext>(handle);
            // initial context
            weak_window.lock()->graphicsContext->Init();
            // call lua onInit
            onInit();
        });
    }
    void beforeAppUpdate() final{
        try {
            beforeUpdate();
        } catch (sol::error &e) {
            std::cout << e.what() << std::endl;
        }
    }
    void onAppUpdate(const SPW::TimeDuration &du) final{
        try {
            onUpdate(du.toSecond());
        } catch (sol::error &e) {
            std::cout << e.what() << std::endl;
        }
    }

    void afterAppUpdate() final{
        try {
            afterUpdate();
        } catch (sol::error &e) {
            std::cout << e.what() << std::endl;
        }
    }
    void onUnConsumedEvents(std::vector<std::shared_ptr<SPW::EventI>> &events) final{
        for (auto &e : events) {
            DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
        }
    }
    void onAppStopped() final{
        try {
            onStop();
        } catch (sol::error &e) {
            std::cout << e.what() << std::endl;
        }
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

    const char *getName() final {return m_name.c_str();}
    std::string m_name;
    std::shared_ptr<SPW::Scene> scene = nullptr;
    sol::state m_state;
    sol::function onInit = sol::nil;
    sol::function beforeUpdate = sol::nil;
    sol::function onUpdate = sol::nil;
    sol::function afterUpdate = sol::nil;
    sol::function onStop = sol::nil;
};

// main entrance
int main(int argc, char **argv) {
    // app test
    auto appProxy =
            SPW::Application::create<GameWrapper>();
    return appProxy->app->run(argc, argv);
}
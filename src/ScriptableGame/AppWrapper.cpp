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
#include "LuaBinding/SceneWrapper.hpp"

#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/ModelComponent.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"

#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"
#include "Platforms/OPENGL/OpenGLBackEnd.h"


#include "EcsFramework/System/RenderSystem/RenderSystem.h"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"
#include "EcsFramework/System/RenderSystem/RenderSystem.h"
#include "EcsFramework/System/AudioSystem/AudioSystem.h"


extern "C"{
#include <luasocket.h>
}

class GameWrapper : public SPW::AppDelegateI {
public:
    explicit GameWrapper(std::shared_ptr<SPW::EventResponderI> &app) :
            SPW::AppDelegateI(app) {
    }
    void onAppInit() final {
        auto window = std::make_shared<SPW::GlfwWindow>();
        app->window = window;

        m_state.open_libraries(sol::lib::base,
                               sol::lib::package,
                               sol::lib::coroutine,
                               sol::lib::string,
                               sol::lib::os,
                               sol::lib::math,
                               sol::lib::table,
                               sol::lib::debug,
                               sol::lib::bit32,
                               sol::lib::io,
                               sol::lib::utf8);
        std::string x = m_state["package"]["path"];
        m_state["package"]["path"] = x + ";./LuaLib/?.lua;./resources/scripts/lua/?.lua";

        // debug
        m_state.require("socket.core",luaopen_socket_core,true);
        m_state.script(R"(require("LuaPanda").start("127.0.0.1", 8818))");
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
            auto cpp_table = m_state["Cpp"].get_or_create<sol::table>();
            cpp_table.new_usertype<SPW::SceneWrapper>("SceneWrapper",
                "createEntity", &SPW::SceneWrapper::createEntity,
                "remove", &SPW::SceneWrapper::remove);
            // create render back end
            int width = weak_window.lock()->frameWidth();
            int height = weak_window.lock()->frameHeight();
            
            cpp_table["SceneWrapper"] = [this, width, height](const std::string &path){
                auto scene = SPW::SceneWrapper(app, path);
                auto renderBackEnd = std::make_shared<SPW::OpenGLBackEnd>();
                scene.m_scene->addSystem(std::make_shared<SPW::AudioSystem>(scene.m_scene));
                scene.m_scene->addSystem(std::make_shared<SPW::RenderSystem>(scene.m_scene, renderBackEnd, width, height));
                scene.m_scene->addSystem(std::make_shared<SPW::KeyControlSystem>(scene.m_scene));
                scene.m_scene->addSystem(std::make_shared<SPW::MouseControlSystem>(scene.m_scene));
                return scene;
            };
            try {
                onInit();
            } catch (sol::error &e) {
                std::cout << e.what() << std::endl;
            }
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
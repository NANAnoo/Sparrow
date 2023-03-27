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

#include "Render/shader.h"

#include "LuaBinding/SceneWrapper.hpp"
#include "LuaBinding/ComponentWrapper.hpp"
#include "LuaBinding/EntityWrapper.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>

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

        // glm::vec3
        {
            // bind multiple constructors for vec3
            auto glm_ns_table = m_state["glm"].get_or_create<sol::table>();
            glm_ns_table.new_usertype<glm::vec3>("vec3",
                    sol::call_constructor,sol::constructors<glm::vec3(const float&, const float&, const float&)>(),
                    "x", &glm::vec3::x,
                    "y", &glm::vec3::y,
                    "z", &glm::vec3::z,
                    "r", &glm::vec3::r,
                    "g", &glm::vec3::g,
                    "b", &glm::vec3::b,
                    sol::meta_function::to_string,[] (const glm::vec3* vec) -> std::string {return glm::to_string(*vec);},
                    sol::meta_function::addition,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)+(*vec_b);},
                    sol::meta_function::subtraction,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)-(*vec_b);},
                    sol::meta_function::multiplication,[] (const glm::vec3* vec,const float a) {return (*vec)*a;},
                    sol::meta_function::division,[] (const glm::vec3* vec,const float a) {return (*vec)/a;},
                    sol::meta_function::unary_minus,[] (const glm::vec3* vec) {return (*vec)*-1.f;},
                    sol::meta_function::equal_to,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)==(*vec_b);}
                    );
            // bind a function to make vec3 from a vec4
            glm_ns_table.set_function("toVec3", [](const glm::vec4& v) { return glm::vec3(v); });
        }

        // glm::vec4
        {
            auto glm_ns_table = m_state["glm"].get_or_create<sol::table>();
            glm_ns_table.new_usertype<glm::vec4>("vec4",sol::call_constructor,sol::constructors<glm::vec4(const float&, const float&, const float&, const float&)>(),
                    "x", &glm::vec4::x,
                    "y", &glm::vec4::y,
                    "z", &glm::vec4::z,
                    "w", &glm::vec4::w,
                    "r", &glm::vec4::r,
                    "g", &glm::vec4::g,
                    "b", &glm::vec4::b,
                    "a", &glm::vec4::a,
                    sol::meta_function::to_string,[] (const glm::vec4* vec) {return glm::to_string(*vec);},
                    sol::meta_function::addition,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)+(*vec_b);},
                    sol::meta_function::subtraction,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)-(*vec_b);},
                    sol::meta_function::multiplication,[] (const glm::vec4* vec,const float a) {return (*vec)*a;},
                    sol::meta_function::division,[] (const glm::vec4* vec,const float a) {return (*vec)/a;},
                    sol::meta_function::unary_minus,[] (const glm::vec4* vec) {return (*vec)*-1.f;},
                    sol::meta_function::equal_to,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)==(*vec_b);}
                    );
        }

        // glm::mat4
        {
            auto glm_ns_table = m_state["glm"].get_or_create<sol::table>();
            glm_ns_table.new_usertype<glm::mat4>("mat4",sol::call_constructor,sol::constructors<glm::mat4(const float&)>(),
                    sol::meta_function::to_string,[] (const glm::mat4* m) {return glm::to_string(*m);},
                    sol::meta_function::addition,[] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)+(*m_b);},
                    sol::meta_function::subtraction,[] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)-(*m_b);},
                    sol::meta_function::multiplication,[] (const glm::mat4* m,const glm::vec4* v) {return (*m)*(*v);},
                    sol::meta_function::division,[] (const glm::mat4* m,const float a) {return (*m)/a;},
                    sol::meta_function::unary_minus,[] (const glm::mat4* m) {return (*m)*-1.f;},
                    sol::meta_function::equal_to,[] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)==(*m_b);}
            );
        }

        // glm functions
        {
            auto glm_ns_table = m_state["glm"].get_or_create<sol::table>();
            glm_ns_table.set_function("rotate",sol::overload([] (const glm::mat4* m,const float f,const glm::vec3* v) {return glm::rotate(*m,f,*v);}));
            glm_ns_table.set_function("radians",sol::overload([] (const float f) {return glm::radians(f);}));
            glm_ns_table.set_function("to_string",sol::overload(
                    [] (const glm::mat4* m) {return glm::to_string((*m));},
                    [] (const glm::vec3* v) {return glm::to_string((*v));}
                    ));
            glm_ns_table.set_function("eulerAngleYXZ",sol::overload(
                [] (const float y,const float x,const float z) {
                    return glm::eulerAngleYXZ(y,x,z);
                    }));
            glm_ns_table.set_function("cross",sol::overload(
                [] (const glm::vec3* v_a,const glm::vec3* v_b) {
                    return glm::cross(*v_a,*v_b);
                    }));
        }

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
                onWindowResize = global_app["onWindowResize"];
                std::string name = global_app["name"];
                int width = global_app["width"];
                int height = global_app["height"];
                app->window->setTitle(name);
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

            // create SPW root table
            auto cpp_table = m_state["SPW"].get_or_create<sol::table>();
            // bind wrappers
            SPW::SceneWrapper::bindLuaTable(cpp_table);
            SPW::EntityWrapper::bindLuaTable(cpp_table);
            SPW::ComponentWrapper::bindLuaTable(cpp_table);

            // bind shaderhandle
            cpp_table.new_usertype<SPW::ShaderHandle>("ShaderHandle",
                    sol::call_constructor, sol::constructors<SPW::ShaderHandle(const std::string&, const std::string&, const std::string&)>(),
                    sol::meta_function::to_string,[] (const SPW::ShaderHandle* shader) {return shader->name;},
                    "name", &SPW::ShaderHandle::name,
                    "vertex", &SPW::ShaderHandle::vertex_shader_path,    
                    "fragment", &SPW::ShaderHandle::frag_shader_path                
            );

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
        // for (auto &e : events) {
        //     DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
        // }
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
        e->dispatch<SPW::WindowResizeType, SPW::WindowEvent>(
                [this](SPW::WindowEvent *e){
                    // resize window
                    try {
                        onWindowResize(e->width, e->height);
                    } catch (sol::error &e) {
                        std::cout << e.what() << std::endl;
                    }
                    return false;
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
    sol::function onWindowResize = sol::nil;
};

// main entrance
int main(int argc, char **argv) {
    // app test
    auto appProxy = SPW::Application::create<GameWrapper>();
    return appProxy->app->run(argc, argv);
}
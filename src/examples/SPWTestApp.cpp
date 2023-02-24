#include <iostream>

#include <sol/sol.hpp>

#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"

#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Control/KeyEvent.hpp"

#include "EcsFramework/Scene.hpp"

#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/ModelComponent.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"


#include "Model/Model.h"

#include "Utils/UUID.hpp"

#include "EcsFramework/System/RenderSystem/RenderSystem.h"
#include "Platforms/OPENGL/OpenGLBackEnd.h"
#include "Platforms/OPENGL/OpenGLxGLFWContext.hpp"

#include "SimpleRender.h"
#include "Control/MouseEvent.hpp"
#include "IO/ResourceManager.h"
#include "Model/Model.h"

class WOC :
        public SPW::WindowEventResponder,
        public SPW::KeyEventResponder,
        public SPW::MouseEventResponder {
public:
    explicit WOC(const std::shared_ptr<SPW::EventResponderI> &parent, const char *name):
            SPW::WindowEventResponder(parent),
            SPW::KeyEventResponder(parent),
            SPW::MouseEventResponder(parent),
            _name(name){
        }
    explicit WOC(const std::shared_ptr<WOC> &parent, const char *name):
            SPW::WindowEventResponder(std::shared_ptr<SPW::WindowEventResponder>(parent)),
            SPW::KeyEventResponder(std::shared_ptr<SPW::KeyEventResponder>(parent)),
            SPW::MouseEventResponder(std::shared_ptr<SPW::MouseEventResponder>(parent)),
            _name(name){
    }
    bool onKeyDown(SPW::KeyEvent *e) override {
        if (_name[0] == 'C') {
            std::cout << "onKeyDown" << std::endl;
            return true;
        }
        return false;
    }
    bool onMouseDown(SPW::MouseEvent *e) override {
        if (_name[0] == 'B') {
            std::cout << "onMouseDown" << std::endl;
            return true;
        }
        return false;
    }
    bool canRespondTo(const std::shared_ptr<SPW::EventI> &e) final {
        return _name[0] != 'E' || e->category() == SPW::MouseCategory;
    }
    const char *_name;
    const char *getName() final {return _name;}
};

std::shared_ptr<SPW::Model> createModel() {

    

    auto tmp = SPW::ResourceManager::getInstance();
    tmp->LoadModel("C:/Dev/Sparrow Renderer/res/Models/hhd.fbx");
	// auto vs = tmp->GetMeshes()[0]->vertices;
    // for(const auto& v: vs)
    // {
    //     std::cout << v.Position.x << v.Position.y << v.Position.z << "\n";
    // }    
    // return tmp;

    return nullptr;
}

// test usage
class Transformer :
        public SPW::WindowEventResponder {
public:
    explicit Transformer (const std::shared_ptr<SPW::EventResponderI> &parent)
            : SPW::WindowEventResponder(parent) {
    }

    bool onWindowResize(int w, int h) override {
        std::cout << "window resize" << "(" << w << ", " << h << ")" <<std::endl;
        bool should_update = false;
        if (w < 500) {
            w = 500;
            should_update = true;
        }
        if (h < 400) {
            h = 400;
            should_update = true;
        }
        if (should_update && ! window.expired())
            window.lock()->setSize(w, h);
        // set projection
        return true;
    }

    // windows size is not equal to frame buffer size
    bool onFrameResize(int w, int h) override {
        glViewport(0, 0, w, h);
        std::cout << "frame resize" << "(" << w << ", " << h << ")" <<std::endl;
        // set projection
        return false;
    }
    const char *getName() final {
        return "Transformer";
    }
    std::weak_ptr<SPW::WindowI> window;
};

class TestDelegate : public SPW::AppDelegateI {
public:
    explicit TestDelegate(std::shared_ptr<SPW::EventResponderI> &app, const char *name) :
            SPW::AppDelegateI(app), _name(name) {
    }
    void onAppInit() final {
        auto window = std::make_shared<SPW::GlfwWindow>();
        app->window = window;
        app->window->setSize(800, 600);
        app->window->setTitle("SPWTestApp");

        transformer = std::make_shared<Transformer>(app->delegate.lock());
        transformer->window = window;

        // weak strong dance
        std::weak_ptr<SPW::GlfwWindow> weak_window = window;
        window->onWindowCreated([weak_window, this](GLFWwindow *handle){
            if (weak_window.expired()) {
                return;
            }
            // create graphics context
            weak_window.lock()->graphicsContext = std::make_shared<SPW::OpenGLxGLFWContext>(handle);
            // initial context
            weak_window.lock()->graphicsContext->Init();

            // create render back end
            renderBackEnd = std::make_shared<SPW::OpenGLBackEnd>();

            // create scene
            scene = SPW::Scene::create(app->delegate.lock());

            // add system
            scene->addSystem(std::make_shared<SPW::RenderSystem>(scene, renderBackEnd));

            // add a camera entity
            auto camera = scene->createEntity("main camera");
            camera->emplace<SPW::TransformComponent>();
            camera->emplace<SPW::CameraComponent>(SPW::PerspectiveType);

            SPW::UUID camera_id = camera->component<SPW::IDComponent>()->getID();

            // add a test game object
            auto triangle = scene->createEntity("test");
            triangle->emplace<SPW::TransformComponent>();

            // add a model to show
            auto model = triangle->emplace<SPW::ModelComponent>(camera_id);
            SPW::ShaderHandle shaderHandle({
                                         "basic",
                                         "./resources/shaders/simpleVs.vert",
                                         "./resources/shaders/simplefrag.frag"
                                     });
            model->modelProgram = shaderHandle;
            model->model = createModel();

            // init scene
            scene->initial();
        });
    }
    void beforeAppUpdate() final{
        scene->beforeUpdate();
    }
    void onAppUpdate(const SPW::TimeDuration &du) final{
        // physics, computation
        scene->onUpdate(du);
    }

    void afterAppUpdate() final{
        scene->afterUpdate();
    }
    void onUnConsumedEvents(std::vector<std::shared_ptr<SPW::EventI>> &events) final{
        for (auto &e : events) {
            DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
        }
    }
    void onAppStopped() final{
        sol::state state;
        state.open_libraries(sol::lib::base, sol::lib::package);
        std::string x = state["package"]["path"];
        state["package"]["path"] = x + ";./resources/scripts/lua/?.lua";
        try {
            if(state.script_file("./resources/scripts/lua/test.lua").valid()) {
                sol::protected_function main_function=state["main"];
                sol::protected_function_result result=main_function();
                if (!result.valid()) {
                    std::cout << "execution error" << std::endl;
                }
            }
        } catch (sol::error &e) {
            std::cout << e.what() << std::endl;
        }
        std::cout << "app stopped" << std::endl;
        scene->onStop();
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
    std::shared_ptr<Transformer> transformer;
    std::shared_ptr<SimpleRender> render;
    std::shared_ptr<SPW::Scene> scene;
    std::shared_ptr<SPW::RenderBackEndI> renderBackEnd;
};

// main entrance
int main(int argc, char **argv) {
    // app test
    auto appProxy =
        SPW::Application::create<TestDelegate>("SPWTestApp");
    return appProxy->app->run(argc, argv);
}
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


std::shared_ptr<SPW::Model> createModel() {
    auto model = std::make_shared<SPW::Model>("");
    std::vector<SPW::Vertex> vertices = {
        {
            {0.0f, 0.5f, 0.0f}, {0, 0, 0}, {0, 0}, {0, 0, 0}, {0, 0, 0}
        },
        {
            {- 0.3f, 0.0f, 0.0f}, {0, 0, 0}, {0.5, 0.5}, {0, 0, 0}, {0, 0, 0}
        },
        {
            {+0.3f, 0.0f, 0.0f}, {0, 0, 0}, {1.0, 0}, {0, 0, 0}, {0, 0, 0}
        }
    };
    SPW::Mesh mesh(vertices, {0, 1, 2});
    mesh.mMaterial->updateTexture(SPW::TextureType::Albedo,"./resources/texture/container.jpg");
    model->meshes.push_back(mesh);
    return model;
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
        // TODO: add a responder to each camera
        scene.lock()->forEach([=](SPW::CameraComponent *cam) {
            cam->aspect = float(w) / float(h);
            if (cam->getType() == SPW::UIOrthoType) {
                cam->right = w;
                cam->top = h;
            }
        }, SPW::CameraComponent);
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
    std::weak_ptr<SPW::Scene> scene;
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
            auto cam = camera->emplace<SPW::CameraComponent>(SPW::PerspectiveType);
            cam->fov = 60;
            cam->aspect = float(weak_window.lock()->width()) / float(weak_window.lock()->height());
            cam->near = 0.01;
            cam->far = 100;

            // add a camera entity
            auto camera2 = scene->createEntity("main camera");
            auto cam2_tran = camera2->emplace<SPW::TransformComponent>();
            cam2_tran->position.y = 0.3;
            cam2_tran->rotation.z = 90;
            auto cam2 = camera2->emplace<SPW::CameraComponent>(SPW::PerspectiveType);
            cam2->fov = 75;
            cam2->aspect = float(weak_window.lock()->width()) / float(weak_window.lock()->height());
            cam2->near = 0.01;
            cam2->far = 100;

            SPW::UUID camera_id = camera->component<SPW::IDComponent>()->getID();
            SPW::UUID camera_id_2 = camera2->component<SPW::IDComponent>()->getID();

            // add a test game object
            auto triangle = scene->createEntity("test");
            auto transform = triangle->emplace<SPW::TransformComponent>();
            transform->scale = {0.5, 0.5, 0.5};

            // add a model to show
            auto model = triangle->emplace<SPW::ModelComponent>(camera_id);
            model->bindCameras.insert(camera_id_2);
            SPW::ShaderHandle shaderHandle({
                                         "basic",
                                         "./resources/shaders/simpleVs.vert",
                                         "./resources/shaders/simplefrag.frag"
                                     });
            model->modelProgram = shaderHandle;
            model->model = createModel();

            // init scene
            scene->initial();
            transformer->scene = scene;
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
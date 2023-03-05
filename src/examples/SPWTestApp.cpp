#include <iostream>

#include <memory>
#include <sol/sol.hpp>

#include "Model/Mesh.h"
#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Control/KeyEvent.hpp"
#include "Control/MouseEvent.hpp"

#include "Control/KeyCodes.h"
#include "Control/MouseCodes.h"

#include "EcsFramework/Scene.hpp"

#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/ModelComponent.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"

#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"

#include "EcsFramework/System/RenderSystem/RenderSystem.h"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"

#include "Model/Model.h"

#include "Utils/UUID.hpp"


#include "EcsFramework/System/RenderSystem/RenderSystem.h"
#include "EcsFramework/System/AudioSystem/AudioSystem.h"
#include "EcsFramework/System/AudioSystem/AudioListenerSystem.h"

#include "Platforms/OPENGL/OpenGLBackEnd.h"
#include "Platforms/OPENGL/OpenGLxGLFWContext.hpp"

#include "SimpleRender.h"
#include "IO/ResourceManager.h"
#include "Model/Model.h"


std::shared_ptr<SPW::Model> createModel() {
    auto model = std::make_shared<SPW::Model>();
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
    std::vector<unsigned int> indices = {0, 1, 2};
    
    auto mesh = std::make_shared<SPW::Mesh>(vertices, indices);
    mesh->mMaterial->updateTexture(SPW::TextureType::Albedo,"./resources/texture/container.jpg");
    model->AddMesh(mesh);
    return model;

    // auto tmp = SPW::ResourceManager::getInstance()->LoadModel("./resources/models/sf_cube/scene.gltf");
	// // auto vs = tmp->GetMeshes()[0]->vertices;
    // // for(const auto& v: vs)
    // // {
    // //     std::cout << v.Position.x << v.Position.y << v.Position.z << "\n";
    // // }    
    // return tmp;
    // return nullptr;
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
            scene->addSystem(std::make_shared<SPW::AudioSystem>(scene));
            scene->addSystem(std::make_shared<SPW::AudioListenerSystem>(scene));
            scene->addSystem(std::make_shared<SPW::KeyControlSystem>(scene));
            scene->addSystem(std::make_shared<SPW::MouseControlSystem>(scene));

            // add a camera entity
            auto camera = scene->createEntity("main camera");
            camera->emplace<SPW::TransformComponent>();
            auto cam = camera->emplace<SPW::CameraComponent>(SPW::PerspectiveType);
            cam->fov = 60;
            cam->aspect = float(weak_window.lock()->width()) / float(weak_window.lock()->height());
            cam->near = 0.01;
            cam->far = 100;

            //add a AudioSource Entity
            auto clip = scene->createEntity("AuidoSource");
            clip->emplace<SPW::TransformComponent>();
            clip->emplace<SPW::AudioComponent>();

            clip->component<SPW::AudioComponent>()->is3D = true;

            //add a Audio Listener
            if(clip->component<SPW::AudioComponent>()->is3D){

                auto listener = scene->createEntity("Listener");
                listener->emplace<SPW::TransformComponent>();
                listener->emplace<SPW::AudioListener>();
                clip->component<SPW::AudioComponent>()->isLoop = true;

            }
            SPW::UUID camera_id = camera->component<SPW::IDComponent>()->getID();

            // add a test game object
            auto triangle = scene->createEntity("test");
            auto transform = triangle->emplace<SPW::TransformComponent>();
            transform->scale = {0.5, 0.5, 0.5};

            //add a key component for testing, press R to rotate
            auto key = triangle->emplace<SPW::KeyComponent>();
            key->onKeyDownCallBack = [transform](const SPW::Entity& e, int keycode){
                if(keycode == static_cast<int>(SPW::Key::R))
                    transform->rotation.y += 5.0f;
            };

            //add a mouse component for testing, press left button to rotate, scroll to scale
            auto mouse = triangle->emplace<SPW::MouseComponent>();
            mouse->cursorMovementCallBack = [](const SPW::Entity& e, double x_pos, double y_pos, double x_pos_bias, double y_pos_bias){
                auto transform = e.component<SPW::TransformComponent>();
                transform->rotation.x += y_pos_bias;
                transform->rotation.y += x_pos_bias;

                // transform->position.x = x_pos;
                // transform->position.y = y_pos;
            };
            mouse->onMouseScrollCallBack = [](const SPW::Entity& e, double scroll_offset){

                auto transform = e.component<SPW::TransformComponent>();
                transform->scale.x += scroll_offset;
                transform->scale.y += scroll_offset;
                transform->scale.z += scroll_offset;
            };

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
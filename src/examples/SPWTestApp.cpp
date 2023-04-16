#include <cmath>
#include <iostream>

#include <memory>
#include <sol/sol.hpp>

#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Entity/Entity.hpp"
#include "Model/Mesh.h"
#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include "Model/Animation/Skeleton.h"
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
#include "EcsFramework/Component/AnimationComponent/AnimationComponent.h"


#include "EcsFramework/System/RenderSystem/RenderSystem.h"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"
#include "EcsFramework/System/AnimationSystem/AnimationSystem.h"
#include "Render/StorageBuffer.h"

#include "Model/Model.h"

#include "Utils/UUID.hpp"


#include "EcsFramework/System/RenderSystem/RenderSystem.h"
#include "EcsFramework/System/AudioSystem/AudioSystem.h"

#include "Platforms/OPENGL/OpenGLBackEnd.h"
#include "Platforms/OPENGL/OpenGLxGLFWContext.hpp"

#include "SimpleRender.h"
#include "IO/ResourceManager.h"
#include "Model/Model.h"
#include "Render/StorageBuffer.h"
#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>


std::shared_ptr<SPW::Model> createModel() {
  /*
     * Animation Test:
     * 1. Bones
     * 2. Animation Clips
     * TODO: Pointial Problems:
     * 1. fbx 读不出权重和父子关系，gltf都可以。 建议尽量使用gltf模型，如果是一定要用的模型，建议在导入工程之前增加一步blender操作。
     * 2. 骨骼父子关系按照aibone in assimp 的方式读， aibone在一个mesh里面，导致读出来的骨骼根节点，是当前mesh的根节点。
     * 3. weight关系（已在沙盒实现vertex[BoneSlot<4>, weight<4>]映射关系）。
     * */
  auto animInstance = SPW::ResourceManager::getInstance()->LoadAnimation("./resources/models/Standing 2H Magic Attack 01.fbx");

  return SPW::ResourceManager::getInstance()->LoadModel("./resources/models/Standing 2H Magic Attack 01.fbx");
}

std::shared_ptr<SPW::Skeleton> createSkeleton() {
    return SPW::ResourceManager::getInstance()->LoadAnimation("./resources/models/Standing 2H Magic Attack 01.fbx");
}
std::shared_ptr<SPW::Model> createCubeModel()
{
    return SPW::ResourceManager::getInstance()->LoadModel("./resources/models/cube.obj");
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

static std::vector<glm::vec4> sColors = {{1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}};
static std::shared_ptr<SPW::StorageBuffer> testColor = std::make_shared<SPW::StorageBuffer>("TestColor", sizeof(glm::vec4) * sColors.size(), 8);

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
            scene->addSystem(std::make_shared<SPW::AudioSystem>(scene));
            scene->addSystem(std::make_shared<SPW::RenderSystem>(scene, renderBackEnd, weak_window.lock()->frameWidth(), weak_window.lock()->frameHeight()));
            scene->addSystem(std::make_shared<SPW::KeyControlSystem>(scene));
            scene->addSystem(std::make_shared<SPW::MouseControlSystem>(scene));
            scene->addSystem(std::make_shared<SPW::AnimationSystem>(scene));

            // add a camera entity
            auto camera = scene->createEntity("main camera");
            camera->emplace<SPW::AudioListener>();
            auto mainCameraTrans = camera->emplace<SPW::TransformComponent>();
            mainCameraTrans->position = glm::vec4(0.0f,0.0f,-1.0f,1.0f);
            auto cam = camera->emplace<SPW::CameraComponent>(SPW::PerspectiveType);
            cam->fov = 60;
            cam->aspect = float(weak_window.lock()->width()) / float(weak_window.lock()->height());
            cam->near = 0.01;
            cam->far = 100;

            //add a AudioSource Entity
            auto clip = scene->createEntity("AuidoSource");
            clip->emplace<SPW::TransformComponent>();
            std::vector<std::string> soundPaths = {
                    "./resources/sounds/test.wav"
            };
            clip->emplace<SPW::AudioComponent>(soundPaths);
            clip->component<SPW::AudioComponent>()->setState(soundPaths[0], SPW::Play);
            clip->component<SPW::AudioComponent>()->setLoop(soundPaths[0], true);
            clip->component<SPW::AudioComponent>()->set3D(soundPaths[0], true);
            auto keyCom =  clip->emplace<SPW::KeyComponent>();
            keyCom->onKeyDownCallBack = [soundPaths](const SPW::Entity& e, SPW::KeyCode keycode) {
                if (keycode == SPW::KeyCode::Space) {
                    e.component<SPW::AudioComponent>()->setLoop(soundPaths[0], false);
                    e.component<SPW::AudioComponent>()->setState(soundPaths[0], SPW::Pause);
                }
                if(keycode == SPW::KeyCode::LeftShift){
                    e.component<SPW::AudioComponent>()->setState(soundPaths[0], SPW::Continue);
                }
                if(keycode == SPW::KeyCode::P){
                    e.component<SPW::AudioComponent>()->setState(soundPaths[0], SPW::Play);
                }
                if(keycode == SPW::KeyCode::O){
                    e.component<SPW::AudioComponent>()->setState(soundPaths[0], SPW::Stop);
                }
            };

            SPW::UUID camera_id = camera->component<SPW::IDComponent>()->getID();
            cam->whetherMainCam = true;
            //add a key component for testing, press R to rotate
            auto cameraKey = camera->emplace<SPW::KeyComponent>();
            auto cb = [](const SPW::Entity& e, SPW::KeyCode keycode){
                auto mainCameraTrans = e.component<SPW::TransformComponent>();
                auto rotMat = glm::eulerAngleYXZ(glm::radians(mainCameraTrans->rotation.y),
                                   glm::radians(mainCameraTrans->rotation.x),
                                   glm::radians(mainCameraTrans->rotation.z));
                glm::vec4 front = {0, 0, -1, 0};
                front = rotMat * front;
                glm::vec3 forward = {front.x, 0, front.z};
                forward = glm::normalize(forward);
                glm::vec3 up = {0, 1, 0};
                glm::vec3 right = glm::normalize(glm::cross(forward, up));
                if(keycode == SPW::Key::W)
                    mainCameraTrans->position +=0.1f * forward;
                if(keycode == SPW::Key::S)
                    mainCameraTrans->position -=0.1f * forward;
                if(keycode == SPW::Key::A)
                    mainCameraTrans->position -=0.1f * right;
                if(keycode == SPW::Key::D)
                    mainCameraTrans->position +=0.1f * right;
                if(keycode == SPW::Key::Q)
                    mainCameraTrans->position -=0.1f * up;
                if(keycode == SPW::Key::E)
                    mainCameraTrans->position +=0.1f * up;
            };
            auto mouse = camera->emplace<SPW::MouseComponent>();
            mouse->cursorMovementCallBack = [](const SPW::Entity& e, double x_pos, double y_pos, double x_pos_bias, double y_pos_bias){
                auto transform = e.component<SPW::TransformComponent>();
                transform->rotation.x -= y_pos_bias * 0.02;
                transform->rotation.y -= x_pos_bias * 0.1 ;
            };
            cameraKey->onKeyHeldCallBack = cb;
            cameraKey->onKeyDownCallBack = cb;

            // add a test game object
            SPW::ShaderHandle ShadowShaderHandle({
                                                   "shadow",
                                                   "./resources/shaders/shadowMap.vert",
                                                   "./resources/shaders/shadowMap.frag"
                                           });

            auto obj = scene->createEntity("test");
            auto transform = obj->emplace<SPW::TransformComponent>();
            transform->scale = {0.01, 0.01, 0.01};
            transform->rotation = {0, 90, 0};
            transform->position = {0, -0.3, 0};

            //add a key component for testing, press R to rotate
            auto key = obj->emplace<SPW::KeyComponent>();
            key->onKeyHeldCallBack = [transform](const SPW::Entity& e, SPW::KeyCode keycode){
                if(keycode == SPW::Key::R)
                    transform->rotation.y += 5.0f;
            };

            mouse->onMouseScrollCallBack = [](const SPW::Entity& e, double scroll_offset){
                auto transform = e.component<SPW::TransformComponent>();
                transform->position.z += scroll_offset * 0.1;
            };

            // add a model to show
            auto model = obj->emplace<SPW::ModelComponent>(camera_id);
            //model->bindCameras.insert(camera_id_2);
            SPW::ShaderHandle shaderHandle({
                                         "basic",
                                         "./resources/shaders/simpleVs.vert",
                                         "./resources/shaders/pbrShadow.frag"
                                     });

            model->modelProgram = shaderHandle;
            model->shadowProgram = ShadowShaderHandle;
            model->model = createModel();
            auto animation = obj->emplace<SPW::AnimationComponent>(createSkeleton());
            animation->skeleton = createSkeleton();
            animation->swapCurrentAnim("mixamo.com");
            //animation->incomingAnimName = "mantis_anim";
            //animation->currentAnimation = animation->skeleton->animClips[0];

            testColor->updateSubData(sColors.data(), 0, sColors.size() * sizeof(glm::vec4));
            model->preRenderCommands.pushCommand(SPW::RenderCommand(&SPW::RenderBackEndI::initStorageBuffer, testColor));
            auto cubeObj = scene->createEntity("floor");
            auto cubeTrans = cubeObj->emplace<SPW::TransformComponent>();
            cubeTrans->scale = {5.0, 0.05, 5.0};
            cubeTrans->position.y-=0.35f;
            auto cubemodel = cubeObj->emplace<SPW::ModelComponent>(camera_id);
            SPW::ShaderHandle CubeshaderHandle({
                                                   "basic",
                                                   "./resources/shaders/simpleVs.vert",
                                                   "./resources/shaders/pbrShadow.frag"
                                           });
            //model->bindCameras.insert(camera_id_2);
            cubemodel->modelProgram = CubeshaderHandle;
            cubemodel->shadowProgram = ShadowShaderHandle;
            cubemodel->model = createCubeModel();

            // add light 1
            auto light = scene->createEntity("light");
            auto lightTrans =light->emplace<SPW::TransformComponent>();
            auto lightCom = light->emplace<SPW::DirectionalLightComponent>();
            lightCom->ambient = {0.2, 0.2, 0.2};
            lightCom->diffuse = {1, 1, 0};
            lightCom->specular = {1, 1, 0};
            lightTrans->rotation = {30, 60, 0};

            // add light 2
            auto light2 = scene->createEntity("light2");
            auto lightTrans2 =light2->emplace<SPW::TransformComponent>();
            auto lightCom2 = light2->emplace<SPW::DirectionalLightComponent>();
            lightCom2->ambient = {0.2, 0.2, 0.2};
            lightCom2->diffuse = {0, 1, 1};
            lightCom2->specular = {0, 1, 1};
            lightTrans2->rotation = {30, 0, 0};

            light2->emplace<SPW::KeyComponent>()->onKeyHeldCallBack =
            [](const SPW::Entity &en, SPW::KeyCode code) {
                if (code == SPW::KeyCode::Up) {
                    en.component<SPW::TransformComponent>()->rotation.x --;
                } else if (code == SPW::KeyCode::Down) {
                    en.component<SPW::TransformComponent>()->rotation.x ++;
                } else if (code == SPW::KeyCode::Left) {
                    en.component<SPW::TransformComponent>()->rotation.y ++;
                } else if (code == SPW::KeyCode::Right) {
                    en.component<SPW::TransformComponent>()->rotation.y --;
                }
            };

            // init scene
            scene->initial();
            transformer->scene = scene;
        });
    }
    void beforeAppUpdate() final{
        scene->beforeUpdate();
        scene->forEachEntity<SPW::ModelComponent>([](const SPW::Entity &en){
            en.component<SPW::ModelComponent>()->pipeLineCommands.pushCommand(SPW::RenderCommand(&SPW::Shader::setStorageBuffer, testColor));
        });
    }
    void onAppUpdate(const SPW::TimeDuration &du) final{
        // physics, computation
        scene->onUpdate(du);
    }

    void afterAppUpdate() final{
        scene->afterUpdate();
    }
    void onUnConsumedEvents(std::vector<std::shared_ptr<SPW::EventI>> &events) final{
        // for (auto &e : events) {
        //     DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
        // }
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
        std::cout << "app Stopped" << std::endl;
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
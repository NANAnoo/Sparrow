#include <cmath>
#include <iostream>

#include <memory>
#include <sol/sol.hpp>

#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/System/AnimationSystem/AnimationSystem.h"
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
#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"

#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"


#include "EcsFramework/System/NewRenderSystem/SPWRenderSystem.h"
#include "EcsFramework/System/NewRenderSystem/DefaultRenderPass.hpp"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"

#include "Model/Model.h"

#include "Utils/UUID.hpp"

#include "EcsFramework/System/AudioSystem/AudioSystem.h"

#include "Platforms/OPENGL/OpenGLBackEnd.h"
#include "Platforms/OPENGL/OpenGLxGLFWContext.hpp"

#include "SimpleRender.h"
#include "IO/ResourceManager.h"
#include "Model/Model.h"
#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>

std::shared_ptr<SPW::Skeleton> createSkeleton() {
    return SPW::ResourceManager::getInstance()->LoadAnimation("./resources/models/dragon/scene.gltf");
}

std::shared_ptr<SPW::Model> createModel() {
    //return SPW::ResourceManager::getInstance()->LoadModel("./resources/models/mona2/mona.fbx");
    return SPW::ResourceManager::getInstance()->LoadModel("./resources/models/dragon/scene.gltf");
}
std::shared_ptr<SPW::Model> createCubeModel()
{
    return SPW::ResourceManager::getInstance()->LoadModel("./resources/models/sand_cube/cube.obj");
}

const SPW::UUID& createMaincamera(const std::shared_ptr<SPW::Scene> &scene, float width, float height) {
    // add a camera entity
    auto camera = scene->createEntity("main camera");
    camera->emplace<SPW::AudioListener>();
    auto mainCameraTrans = camera->emplace<SPW::TransformComponent>();
    mainCameraTrans->position = glm::vec4(0.0f,0.0f,1.0f,1.0f);
    auto cam = camera->emplace<SPW::CameraComponent>(SPW::PerspectiveType);
    cam->fov = 60;
    cam->aspect = width / height;
    cam->near = 0.01;
    cam->far = 100;

    cam->whetherMainCam = true;
    //add a key component for testing, press R to rotate
    auto cameraKey = camera->emplace<SPW::KeyComponent>();
    static float speed = 0.02f;
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
            mainCameraTrans->position +=speed * forward;
        if(keycode == SPW::Key::S)
            mainCameraTrans->position -=speed * forward;
        if(keycode == SPW::Key::A)
            mainCameraTrans->position -=speed * right;
        if(keycode == SPW::Key::D)
            mainCameraTrans->position +=speed * right;
        if(keycode == SPW::Key::Q)
            mainCameraTrans->position -=speed * up;
        if(keycode == SPW::Key::E)
            mainCameraTrans->position +=speed * up;
        if (keycode == SPW::Key::LeftShift) {
            speed = 0.1f;
        }
    };
    auto mouse = camera->emplace<SPW::MouseComponent>();
    mouse->cursorMovementCallBack = [](const SPW::Entity& e, double x_pos, double y_pos, double x_pos_bias, double y_pos_bias){
        auto transform = e.component<SPW::TransformComponent>();
        transform->rotation.x -= y_pos_bias * 0.1;
        transform->rotation.y -= x_pos_bias * 0.1 ;
    };
    cameraKey->onKeyHeldCallBack = cb;
    cameraKey->onKeyDownCallBack = cb;
    cameraKey->onKeyReleasedCallBack = [](const SPW::Entity& e, SPW::KeyCode keycode) {
        if (keycode == SPW::Key::LeftShift) {
            speed = 0.02f;
        }
    };
    return camera->component<SPW::IDComponent>()->getID();
}

std::shared_ptr<SPW::Entity> createPlight(const std::shared_ptr<SPW::Scene> &scene, glm::vec3 position, glm::vec3 color) {
    auto light = scene->createEntity("light");
    auto lightTrans =light->emplace<SPW::TransformComponent>();
    auto lightCom = light->emplace<SPW::PointLightComponent>();
    lightCom->ambient = color;
    lightTrans->position = position;
    return light;
}

std::shared_ptr<SPW::Entity> createDlight(const std::shared_ptr<SPW::Scene> &scene, glm::vec3 rotation, glm::vec3 color) {
    auto light = scene->createEntity("light");
    auto lightTrans =light->emplace<SPW::TransformComponent>();
    auto lightCom = light->emplace<SPW::DirectionalLightComponent>();
    lightCom->ambient = color;
    lightTrans->rotation = rotation;
    return light;
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
        app->window->setSize(1280, 720);
        app->window->setTitle("Test");

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

            // create render system
            auto rendersystem = std::make_shared<SPW::SPWRenderSystem>(scene, renderBackEnd, weak_window.lock()->frameWidth(), weak_window.lock()->frameHeight());
            // add system
            scene->addSystem(std::make_shared<SPW::AudioSystem>(scene));
            scene->addSystem(rendersystem);
            scene->addSystem(std::make_shared<SPW::KeyControlSystem>(scene));
            scene->addSystem(std::make_shared<SPW::MouseControlSystem>(scene));
            scene->addSystem(std::make_shared<SPW::AnimationSystem>(scene));

            // ------ create main render graph ----------------
            auto pbr_with_PDshadow = rendersystem->createRenderGraph();

            auto p_shadowmap_node = pbr_with_PDshadow->createRenderNode<SPW::ModelRepeatPassNode>(SPW::CubeMapType, SPW::RepeatForPLights, 10);
            p_shadowmap_node->width = 256;
            p_shadowmap_node->height = 256;
            p_shadowmap_node->clearType = SPW::ClearDepth;


            auto d_shadowmap_node = pbr_with_PDshadow->createRenderNode<SPW::ModelRepeatPassNode>(SPW::ColorType, SPW::RepeatForDLights, 10);
            d_shadowmap_node->width = 4096;
            d_shadowmap_node->height = 4096;
            d_shadowmap_node->clearType = SPW::ClearDepth;

            auto pbr_shadow_lighting_node = pbr_with_PDshadow->createRenderNode<SPW::ModelToScreenNode>();

            auto p_shadowmap_output = p_shadowmap_node->addAttachment(SPW::Depth);
            auto d_shadowmap_output = d_shadowmap_node->addAttachment(SPW::Depth);

            pbr_shadow_lighting_node->bindInputPort(p_shadowmap_output);
            pbr_shadow_lighting_node->bindInputPort(d_shadowmap_output);
            
            auto pbr_shadow_lighting_output = pbr_shadow_lighting_node->addScreenAttachment(SPW::ScreenColorType);
            // ------ create main render graph ----------------

            // ------ create post processing graph --------------
            SPW::AttachmentPort screen_color_port = {SPW::SCREEN_PORT, SPW::ScreenColorType};
            rendersystem->presentNode = rendersystem->postProcessGraph->createRenderNode<SPW::PresentNode>(FXAA_desc(screen_color_port));
            rendersystem->presentNode->bindInputPort(screen_color_port);
            rendersystem->presentNode->depthTest = false;
            // ------ create post processing graph --------------

            // --------------- create shader ---------------
            SPW::ShaderHandle pbr_light_shadow({
                                         "pbr_light_shadow",
                                         "./resources/shaders/simpleVs.vert",
                                         "./resources/shaders/pbrShadow.frag"
                                     });
            SPW::ShaderHandle pbr_ani_light_shadow({
                                         "pbr_light_shadow",
                                         "./resources/shaders/ani_model.vert",
                                         "./resources/shaders/pbrShadow.frag"
                                     });
            SPW::ShaderHandle pbr_light_shadow_tiled({
                                            "pbr_light_shadow_tiled",
                                            "./resources/shaders/simpleVs.vert",
                                            "./resources/shaders/pbrShadowTiled.frag"
                                    });
            auto p_shadow_desc = SPW::P_shadowmap_desc();
            auto d_shadow_desc = SPW::D_shadowmap_desc();
            auto p_ani_shadow_desc = SPW::P_ani_shadowmap_desc();
            auto d_ani_shadow_desc = SPW::D_ani_shadowmap_desc();

            auto pbr_ani_light_shadow_desc = PBR_ani_shadow_desc(p_shadowmap_output, d_shadowmap_output, pbr_ani_light_shadow);
            auto pbr_light_shadow_desc = PBR_light_with_shadow_desc(p_shadowmap_output, d_shadowmap_output, pbr_light_shadow);
            auto pbr_light_shadow_tiled_desc = PBR_light_with_shadow_desc(p_shadowmap_output, d_shadowmap_output, pbr_light_shadow_tiled);
            
            auto skybox_desc = SPW::SkyBoxShader_desc();
            rendersystem->addShaderDesciptor(pbr_light_shadow_desc);
            rendersystem->addShaderDesciptor(pbr_light_shadow_tiled_desc);
            rendersystem->addShaderDesciptor(p_shadow_desc);
            rendersystem->addShaderDesciptor(d_shadow_desc);
            rendersystem->addShaderDesciptor(skybox_desc);
            rendersystem->addShaderDesciptor(p_ani_shadow_desc);
            rendersystem->addShaderDesciptor(d_ani_shadow_desc);
            rendersystem->addShaderDesciptor(pbr_ani_light_shadow_desc);

            // --------------- create shader ---------------
            auto camera_id = createMaincamera(scene, weak_window.lock()->width(), weak_window.lock()->height());
            // --------------------------------------------------------------------------------

            auto obj = scene->createEntity("test");
            auto transform = obj->emplace<SPW::TransformComponent>();
            transform->scale = {0.05, 0.05, 0.05};
            transform->rotation = {0, 90, 0};
            transform->position = {0, -0.3, 0};

            // add a model to show
            auto model = obj->emplace<SPW::MeshComponent>(camera_id);

            model->bindRenderGraph = pbr_with_PDshadow->graph_id;
            model->modelSubPassPrograms[p_shadowmap_node->pass_id] = p_ani_shadow_desc.uuid;
            model->modelSubPassPrograms[d_shadowmap_node->pass_id] = d_ani_shadow_desc.uuid;
            model->modelSubPassPrograms[pbr_shadow_lighting_node->pass_id] = pbr_ani_light_shadow_desc.uuid;

            model->model = createModel();
            auto animation = obj->emplace<SPW::AnimationComponent>(createSkeleton(),model->model);
            //animation->swapCurrentAnim("mixamo.com");
            animation->swapCurrentAnim("Scene");

            // --------------------------------------------------------------------------------
            auto cubeObj = scene->createEntity("floor");
            auto cubeTrans = cubeObj->emplace<SPW::TransformComponent>();
            cubeTrans->scale = {5.0, 0.05, 5.0};
            cubeTrans->position.y-=0.35f;
            auto cubemodel = cubeObj->emplace<SPW::MeshComponent>(camera_id);
            cubemodel->model = createCubeModel();

            cubemodel->bindRenderGraph = pbr_with_PDshadow->graph_id;
            cubemodel->modelSubPassPrograms[pbr_shadow_lighting_node->pass_id] = pbr_light_shadow_tiled_desc.uuid;

            // --------------------------------------------------------------------------------
            auto skybox = scene->createEntity("skybox");
            auto skyboxTrans = skybox->emplace<SPW::TransformComponent>();
            auto skyMesh = skybox->emplace<SPW::MeshComponent>(camera_id);
            skyMesh->model = SPW::createSkyBoxModel({
                "./resources/texture/skybox/right.jpg",
                "./resources/texture/skybox/left.jpg",
                "./resources/texture/skybox/top.jpg",
                "./resources/texture/skybox/bottom.jpg",
                "./resources/texture/skybox/front.jpg",
                "./resources/texture/skybox/back.jpg"
            });
            skyMesh->bindRenderGraph = rendersystem->skyBoxGraph->graph_id;
            skyMesh->modelSubPassPrograms[rendersystem->skyBoxNode->pass_id] = skybox_desc.uuid;

            auto light1 = createPlight(scene, {1, 1, 0}, {1, 0.5, 0});
            auto light2 = createPlight(scene, {-1, 1, 0}, {0, 0.5, 1});
            auto light3 = createDlight(scene, {30, 60, 0}, {0.5, 0, 1});
            auto light4 = createDlight(scene, {30, -60, 0}, {0.5, 1, 0});
            static int control_id = 0;
            auto light_controller = [](int idx){
                return [idx](const SPW::Entity &en, SPW::KeyCode code) {
                    if (code == SPW::KeyCode::D1) {
                        control_id = 0;
                    } else if (code == SPW::KeyCode::D2){
                        control_id = 1;
                    } else if (code == SPW::KeyCode::D3){
                        control_id = 2;
                    } else if (code == SPW::KeyCode::D4){
                        control_id = 3;
                    } 
                    if (control_id == idx) {
                        if ((control_id == 1 || control_id == 0)) {
                            if (code == SPW::KeyCode::Up) {
                                en.component<SPW::TransformComponent>()->position.z -= 0.1;
                            } else if (code == SPW::KeyCode::Down) {
                                en.component<SPW::TransformComponent>()->position.z += 0.1;
                            } else if (code == SPW::KeyCode::Left) {
                                en.component<SPW::TransformComponent>()->position.x -= 0.1;
                            } else if (code == SPW::KeyCode::Right) {
                                en.component<SPW::TransformComponent>()->position.x += 0.1;
                            }
                        } else {
                            if (code == SPW::KeyCode::Up) {
                                en.component<SPW::TransformComponent>()->rotation.x -= 5;
                            } else if (code == SPW::KeyCode::Down) {
                                en.component<SPW::TransformComponent>()->rotation.x += 5;
                            } else if (code == SPW::KeyCode::Left) {
                                en.component<SPW::TransformComponent>()->rotation.y += 5;
                            } else if (code == SPW::KeyCode::Right) {
                                en.component<SPW::TransformComponent>()->rotation.y -= 5;
                            }
                        }
                    }
                };
            };
            light1->emplace<SPW::KeyComponent>()->onKeyHeldCallBack = light_controller(0);
            light2->emplace<SPW::KeyComponent>()->onKeyHeldCallBack = light_controller(1);
            light3->emplace<SPW::KeyComponent>()->onKeyHeldCallBack = light_controller(2);
            light4->emplace<SPW::KeyComponent>()->onKeyHeldCallBack = light_controller(3);

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
        // for (auto &e : events) {
        //     DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
        // }
    }
    void onAppStopped() final{
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
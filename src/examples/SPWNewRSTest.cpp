#include <iostream>

#include <memory>

#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Entity/Entity.hpp"
#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include <glad/glad.h>


#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Control/KeyCodes.h"

#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/PhysicalComponent/RigidActor.h"

#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/PhysicSystem/PhysicSystem.h"
#include "Render/RenderGraphManager.h"

#include "Utils/UUID.hpp"

#include "EcsFramework/System/AudioSystem/AudioSystem.h"

#include "Platforms/OPENGL/OpenGLBackEnd.h"
#include "Platforms/OPENGL/OpenGLxGLFWContext.hpp"

#include "SimpleRender.h"
#include "Asset/ResourceManager/ResourceManager.h"
#include <glm/glm/gtx/euler_angles.hpp>

#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/System/NewRenderSystem/SPWRenderSystem.h"
#include "IO/FileSystem.h"
#include "Asset/Serializer/EntitySerializer.h"
#include "Asset/BasicMeshStorage.hpp"
#include "EcsFramework/System/AnimationSystem/AnimationSystem.h"
#include "IO/ConfigManager.h"
#include "UI/UIButton.hpp"
#include "UI/UILabel.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"


const SPW::UUID& CreateACamera(const std::shared_ptr<SPW::Scene>& scene, float width, float height, bool main = true)
{
	// add a camera entity
	auto camera = scene->createEntity("main camera");
	camera->emplace<SPW::AudioListener>();
	auto mainCameraTrans = camera->emplace<SPW::TransformComponent>();
	mainCameraTrans->position = glm::vec4(4.0f, 3.0f, 1.0f, 1.0f);
	auto cam = camera->emplace<SPW::CameraComponent>(SPW::PerspectiveType);
	cam->fov = 60;
	cam->aspect = width / height;
	cam->near = 0.1;
	cam->far = 100;

    cam->whetherMainCam = main;
    static bool speed_up = false;
    //add a key component for testing, press R to rotate
    auto cameraKey = camera->emplace<SPW::KeyComponent>();
    auto cb = [](const SPW::Entity &e, SPW::KeyCode keycode) {
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
        if (keycode == SPW::Key::W)
            mainCameraTrans->position += (speed_up ? 0.1f : 0.01f) * forward;
        if (keycode == SPW::Key::S)
            mainCameraTrans->position -= (speed_up ? 0.1f : 0.01f) * forward;
        if (keycode == SPW::Key::A)
            mainCameraTrans->position -= (speed_up ? 0.1f : 0.01f) * right;
        if (keycode == SPW::Key::D)
            mainCameraTrans->position += (speed_up ? 0.1f : 0.01f) * right;
        if (keycode == SPW::Key::Q)
            mainCameraTrans->position -= (speed_up ? 0.1f : 0.01f) * up;
        if (keycode == SPW::Key::E)
            mainCameraTrans->position += (speed_up ? 0.1f : 0.01f) * up;
        if (keycode == SPW::Key::LeftShift)
            speed_up = true;
    };
    auto mouse = camera->emplace<SPW::MouseComponent>();
    mouse->cursorMovementCallBack = [](const SPW::Entity &e, double x_pos, double y_pos, double x_pos_bias,
                                       double y_pos_bias) {
        auto transform = e.component<SPW::TransformComponent>();
        transform->rotation.x -= float(y_pos_bias * 0.1);
        transform->rotation.y -= float(x_pos_bias * 0.1);
    };
    cameraKey->onKeyHeldCallBack = cb;
    cameraKey->onKeyDownCallBack = cb;
    cameraKey->onKeyReleasedCallBack = [](const SPW::Entity &e, SPW::KeyCode keycode) {
        if (keycode == SPW::Key::LeftShift)
            speed_up = false;
    };
    return camera->component<SPW::IDComponent>()->getID();
}

std::shared_ptr<SPW::Entity>
createPlight(const std::shared_ptr<SPW::Scene> &scene, glm::vec3 position, glm::vec3 color) {
    auto light = scene->createEntity("light");
    auto lightTrans = light->emplace<SPW::TransformComponent>();
    auto lightCom = light->emplace<SPW::PointLightComponent>();
    lightCom->ambient = color;
    lightTrans->position = position;
    return light;
}

std::shared_ptr<SPW::Entity>
createDirectionalLight(const std::shared_ptr<SPW::Scene> &scene, glm::vec3 rotation, glm::vec3 color) {
    auto light = scene->createEntity("light");
    auto lightTrans = light->emplace<SPW::TransformComponent>();
    auto lightCom = light->emplace<SPW::DirectionalLightComponent>();
    lightCom->ambient = color;
    lightTrans->rotation = rotation;
    return light;
}

// #define SAVE_SKYBOX
// #define LOAD_ASSET

class SPWTestApp : public SPW::AppDelegateI {
public:
    explicit SPWTestApp(std::shared_ptr<SPW::EventResponderI> &app, const char *name)
            : SPW::AppDelegateI(app), _name(name) {
    }

	void onAppInit() final
	{
		// 2. Simulate a process of loading some resources into a scene
		{
			auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + "dragon/dragon.json");
			SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
			// [data.assetID] = std::move(data);
		}

        {
            auto data = SPW::AssetManager::LoadAsset(
                    SPW::Config::k_WorkingProjectAssets + "cube/cube.json");
            SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
        }

        {
            auto data = SPW::AssetManager::LoadAsset( SPW::Config::k_WorkingProjectAssets + "sand_cube/sand_cube.json");
            SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
        }

		{
            auto data = SPW::AssetManager::LoadAsset( SPW::Config::k_WorkingProjectAssets + "dragon/dragon.json");
            SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
        }

		{
			auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + "mantis/mantis.json");
			SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
		}

		{
			auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + "cube/cube.json");
			SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
		}
		{
			auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + "skybox/skybox.json");
			SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
		}
		{
			auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + "cellar/cellar.json");
			SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
		}

        // -------------------------------OFFLINE TEST-------------------------------------------

		std::shared_ptr<SPW::GlfwWindow> window = std::make_shared<SPW::GlfwWindow>();
		app->window = window;
		app->window->setSize(1280, 720);
		app->window->setTitle("New Render System Test");

        // weak strong dance
        std::weak_ptr weak_window = window;
        window->onWindowCreated([weak_window, this](GLFWwindow *handle) {
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
            auto renderSystem = std::make_shared<SPW::SPWRenderSystem>(scene, renderBackEnd,
                                                                       weak_window.lock()->frameWidth(),
                                                                       weak_window.lock()->frameHeight());
            // add system
            scene->m_renderSystem = renderSystem;
            scene->addSystem(std::make_shared<SPW::AudioSystem>(scene));
            scene->addSystem(renderSystem);
            scene->addSystem(std::make_shared<SPW::KeyControlSystem>(scene));
            scene->addSystem(std::make_shared<SPW::MouseControlSystem>(scene));
            scene->addSystem(std::make_shared<SPW::AnimationSystem>(scene));
            scene->addSystem(std::make_shared<SPW::PhysicSystem>(scene));

            // create ui camera
            scene->uiCamera = scene->createEntity("ui_camera");
            auto ui_trans = scene->uiCamera->emplace<SPW::TransformComponent>();
            ui_trans->position = glm::vec3(0, 0, 0);
            auto ui_camera = scene->uiCamera->emplace<SPW::CameraComponent>(SPW::UIOrthoType);
            ui_camera->left = 0;
            ui_camera->right = float(weak_window.lock()->frameWidth());
            ui_camera->bottom = 0;
            ui_camera->top = float(weak_window.lock()->frameHeight());

            // create main camera
            auto camera_id = CreateACamera(scene, float(weak_window.lock()->width()),
                                           float(weak_window.lock()->height()));

            // ------ create render graph ----------------
            SPW::RenderGraphManager::getInstance()->createRenderGraph(renderBackEnd, SPW::kDefferShadingGraph);
            SPW::RenderGraphManager::getInstance()->createRenderGraph(renderBackEnd, SPW::kSkyBoxRenderGraph);
            SPW::RenderGraphManager::getInstance()->forEachShader([&renderSystem](const SPW::ShaderDesc &shader) {
                renderSystem->addShaderDescriptor(shader);
            });

//            auto bar = scene->createEntity("bar");
//			auto bar_transform = bar->emplace<SPW::TransformComponent>();
//            bar_transform->scale = {1, 1, 1};
//            bar_transform->rotation = {0, 0, 0};
//            bar_transform->position = {0, 0, 0};
//
//			auto bar_model = bar->emplace<SPW::MeshComponent>(camera_id);
//            bar_model->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
//            bar_model->modelSubPassPrograms[GET_RENDER_NODE(SPW::kPointShadowNode)] = GET_SHADER_DESC(SPW::kPointShadowShader).uuid;
//            bar_model->modelSubPassPrograms[GET_RENDER_NODE(SPW::kDirectionalShadowNode)] = GET_SHADER_DESC(SPW::kDirectionalShadowShader).uuid;
//            bar_model->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(SPW::kGBufferShader).uuid;
//            bar_model->assetID   = SPW::ResourceManager::getInstance()->m_AssetDataMap["scene"].assetID;
//			bar_model->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["scene"].assetName;
//			bar_model->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["scene"].path;
            // game objects
            // --------------- dragon ---------------
            auto dragon = scene->createEntity("anim dragon");
            auto dragon_transform = dragon->emplace<SPW::TransformComponent>();
            dragon_transform->scale = {1, 1, 1};
            dragon_transform->rotation = {0, 90, 0};
            dragon_transform->position = {0, 0, 0};

			auto dragon_model = dragon->emplace<SPW::MeshComponent>(camera_id);
			dragon_model->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].assetName;
			dragon_model->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].assetID;
			dragon_model->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].path;

			dragon_model->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
			dragon_model->modelSubPassPrograms[GET_RENDER_NODE(SPW::kPointShadowNode)] = GET_SHADER_DESC(SPW::kAniPointShadowShader).uuid;
			dragon_model->modelSubPassPrograms[GET_RENDER_NODE(SPW::kDirectionalShadowNode)] = GET_SHADER_DESC(SPW::kAniDirectionalShadowShader).uuid;
			dragon_model->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(SPW::kAniGBufferShader).uuid;

            // add animation
            auto dragon_anim = dragon->emplace<SPW::AnimationComponent>(
                    SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].assetName);
            dragon_anim->swapCurrentAnim("dragon_idle");

            dragon->emplace<SPW::KeyComponent>()->onKeyDownCallBack
            = [](const SPW::Entity &e, SPW::KeyCode keycode) {
                if (keycode == SPW::KeyCode::D9) {
                    e.component<SPW::TransformComponent>()->position.y += 0.01;
                } else if (keycode == SPW::KeyCode::D6) {
                    e.component<SPW::TransformComponent>()->position.y -= 0.01;
                }
            };

            // --------------------------------------------------------------------------------
            auto mantis = scene->createEntity("mantis");
            auto transform = mantis->emplace<SPW::TransformComponent>();
            transform->scale = {0.2, 0.2, 0.2};
            transform->rotation = {0, 90, 0};
            transform->position = {5, 10, 0};

            // add a model to show
            auto mantis_mesh = mantis->emplace<SPW::MeshComponent>(camera_id);
            mantis_mesh->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["mantis"].assetName;
            mantis_mesh->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["mantis"].assetID;
            mantis_mesh->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["mantis"].path;

            mantis_mesh->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
            mantis_mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kPointShadowNode)] = GET_SHADER_DESC(
                    SPW::kPointShadowShader).uuid;
            mantis_mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kDirectionalShadowNode)] = GET_SHADER_DESC(
                    SPW::kDirectionalShadowShader).uuid;
            mantis_mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(
                    SPW::kGBufferShader).uuid;

            auto rigid1 = mantis->emplace<SPW::RigidDynamicComponent>();
            rigid1->rigidState = SPW::Awake;

            auto collider1 = mantis->emplace<SPW::CapsuleCollider>();
            collider1->capsule_half_height_ = 0.1;
            collider1->capsule_radius_ = 0.1;
            collider1->degree = PxHalfPi;
            collider1->capsule_axis_ = glm::vec3(0, 1, 0);
            collider1->state = SPW::needAwake;
            collider1->is_trigger_ = false;

			//  --------------------------------------------------------------------------------
			auto cubeObj = scene->createEntity("floor");
			auto cubeTrans = cubeObj->emplace<SPW::TransformComponent>();
			cubeTrans->scale = {50.0, 0.05, 50.0};
			auto floorModel = cubeObj->emplace<SPW::MeshComponent>(camera_id);
            floorModel->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["cube"].assetID;
            floorModel->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["cube"].assetName;
            floorModel->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["cube"].path;

            floorModel->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
            floorModel->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(
                    SPW::kFloorGBufferShader).uuid;

            auto rigid2 = cubeObj->emplace<SPW::RigidStaticComponent>();
            rigid2->rigidState = SPW::Awake;
            auto collider2 = cubeObj->emplace<SPW::BoxCollider>();
            collider2->box_size_ = glm::vec3(50, 0.1, 50);
            collider2->state = SPW::needAwake;

            auto wallObj = scene->createEntity("wall");
            auto wallTrans = wallObj->emplace<SPW::TransformComponent>();
            wallTrans->scale = {0.05, 10, 10.0};
            wallTrans->position = {5.5, 10.0, 0};
            auto wallModel = wallObj->emplace<SPW::MeshComponent>(camera_id);
            wallModel->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["sand_cube"].assetID;
            wallModel->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["sand_cube"].assetName;
            wallModel->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["sand_cube"].path;
            wallModel->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
            wallModel->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(SPW::kFloorGBufferShader).uuid;

			// --------------------------------------------------------------------------------
			auto skybox = scene->createEntity("skybox");
            skybox->emplace<SPW::TransformComponent>();
            auto skyMesh = skybox->emplace<SPW::MeshComponent>(camera_id);
            skyMesh->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["skybox"].assetID;
            skyMesh->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["skybox"].assetName;
            skyMesh->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["skybox"].path;

            skyMesh->bindRenderGraph = GET_RENDER_GRAPH(SPW::kSkyBoxRenderGraph);
            skyMesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kSkyboxNode)] = GET_SHADER_DESC(SPW::kSkyBoxShader).uuid;

            auto light1 = createPlight(scene, {5, 2, 2}, {10, 5, 0});
            auto light2 = createPlight(scene, {5, 2, -2}, {0, 5, 10});
            auto light3 = createDirectionalLight(scene, {30, 60, 0}, {0.5, 0, 1});
            auto light4 = createDirectionalLight(scene, {30, -60, 0}, {0.5, 1, 0});
            static int control_id = 0;
            auto light_controller = [](int idx) {
                return [idx](const SPW::Entity &en, SPW::KeyCode code) {
                    if (code == SPW::KeyCode::D1) {
                        control_id = 0;
                    } else if (code == SPW::KeyCode::D2) {
                        control_id = 1;
                    } else if (code == SPW::KeyCode::D3) {
                        control_id = 2;
                    } else if (code == SPW::KeyCode::D4) {
                        control_id = 3;
                    }
                    if (control_id == idx) {
                        if ((control_id == 1 || control_id == 0)) {
                            if (code == SPW::KeyCode::Up) {
                                en.component<SPW::TransformComponent>()->position.z -= 0.1;
                            } else if (code == SPW::KeyCode::Down) {
                                en.component<SPW::TransformComponent>()->position.z += 0.1;
                            } else if (code == SPW::KeyCode::Left) {
                                en.component<SPW::TransformComponent>()->position.x += 0.1;
                            } else if (code == SPW::KeyCode::Right) {
                                en.component<SPW::TransformComponent>()->position.x -= 0.1;
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

            auto test_button2 = new SPW::UIButton(scene->getUIResponder(), scene, "test2");
            test_button2->setButtonOnClick([&](){
                app->stop();
            });
            test_button2->setButtonOnHover([](){
                std::cout << "Button 2 Hovered" << std::endl;
            });
            test_button2->setButtonNoHover([](){
                std::cout << "Button 2 No Hover" << std::endl;
            });
            test_button2->setPosition(50,50);
            test_button2->setSize(100,100);

            // init scene
            scene->initUIResponder(weak_window.lock()->width(), weak_window.lock()->height());
            scene->initial();
        });
    }

    void beforeAppUpdate() final {
        scene->beforeUpdate();
    }

    void onAppUpdate(const SPW::TimeDuration &du) final {
        // physics, computation
        scene->onUpdate(du);
    }

    void afterAppUpdate() final {
        scene->afterUpdate();
    }

    void onUnConsumedEvents(std::vector<std::shared_ptr<SPW::EventI>> &events) final {
        // for (auto &e : events) {
        //     DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
        // }
    }

    void onAppStopped() final {
        std::cout << "app stopped" << std::endl;
        scene->onStop();
    }

    void solveEvent(const std::shared_ptr<SPW::EventI> &e) final {
        e->dispatch<SPW::WindowCloseType, SPW::WindowEvent>(
                [this](SPW::WindowEvent *e) {
                    // close application

                    app->stop();
                    return true;
                });
        e->dispatch<SPW::WindowFrameResizeType, SPW::WindowEvent>(
                [this](SPW::WindowEvent *e) {
                    // resize cameras
                    scene->forEach(
                        [=](SPW::CameraComponent* cam) 
                        {
                            if (cam->getType() == SPW::UIOrthoType) {
                                cam->right = float(e->width);
                                cam->top = float(e->height);
                            } else {
                                cam->aspect = float(e->width) / float(e->height);
                            }
                        }, SPW::CameraComponent);
                    
                    return true;
                });
        e->dispatch<SPW::MouseDownType, SPW::MouseEvent>(
                [this](SPW::MouseEvent *e) {
                    static bool enabled = false;
                    if (e->button_code == SPW::MouseCode::ButtonRight) {
                        enabled = !enabled;
                        this->scene->isUIMode = enabled;
                        app->window->enableCursor(enabled);
                    }
                    return true;
                }
        );
        SPW::EventResponderI::solveEvent(e);
    }

    const char *getName() final { return _name; }

    const char *_name;
    std::shared_ptr<SimpleRender> render;
    std::shared_ptr<SPW::Scene> scene;
    std::shared_ptr<SPW::RenderBackEndI> renderBackEnd;
};

// main entrance
int main(int argc, char** argv)
{
	if (SPW::ConfigManager::Boost())
		std::cout << "Successfully Boost" << std::endl;

	// app test
	auto appProxy = SPW::Application::create<SPWTestApp>("SPWTestApp");
	return appProxy->app->run(argc, argv);
}

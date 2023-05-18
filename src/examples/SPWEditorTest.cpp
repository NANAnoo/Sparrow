#include <iostream>
#include <memory>
#include <sol/sol.hpp>
#include <glm/glm/gtx/euler_angles.hpp>

#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include <glad/glad.h>

#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Control/KeyCodes.h"
// platform
#include "Platforms/OPENGL/OpenGLBackEnd.h"
#include "Platforms/OPENGL/OpenGLxGLFWContext.hpp"
// ecs
#include "EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/PhysicalComponent/RigidActor.h"
#include "EcsFramework/Component/PhysicalComponent/Collider.h"
#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/System/AudioSystem/AudioSystem.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/PhysicSystem/PhysicSystem.h"
#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/System/NewRenderSystem/SPWRenderSystem.h"
#include "EcsFramework/Component/AnimationComponent/AnimationComponent.h"
#include "EcsFramework/System/AnimationSystem/AnimationSystem.h"
#include "EcsFramework/Component/PhysicalComponent/RigidActor.h"
// render
#include "Render/DefaultRenderPass.hpp"
#include "Render/RenderGraphManager.h"
#include "SimpleRender.h"
#include "Utils/UUID.hpp"
// io
#include "IO/FileSystem.h"
#include "IO/ConfigManager.h"
#include "IO/LogSystem/LogSystem.hpp"
// asset
#include "Asset/Serializer/EntitySerializer.h"
#include "Asset/ResourceManager/ResourceManager.h"
// ui
#include "ImGui/ImGuiEvent.hpp"
#include "ImGui/ImGuiManager.hpp"

auto CreateEmptyNode(const std::shared_ptr<SPW::Scene>& scene) -> std::shared_ptr<SPW::Entity>
{
	return scene->createEntity("emptyNode");
}

void CreateAProject()
{
}

const SPW::UUID& CreateACamera(const std::shared_ptr<SPW::Scene>& scene, float width, float height, bool main = true)
{
	// add a camera entity
	auto camera = scene->createEntity("main camera");
	camera->emplace<SPW::AudioListener>();
	auto mainCameraTrans = camera->emplace<SPW::TransformComponent>();
	mainCameraTrans->position = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	auto cam = camera->emplace<SPW::CameraComponent>(SPW::PerspectiveType);
	cam->fov = 60;
	cam->aspect = width / height;
	cam->near = 0.01;
	cam->far = 100;

	cam->whetherMainCam = main;
	//add a key component for testing, press R to rotate
	auto cameraKey = camera->emplace<SPW::KeyComponent>();
	auto cb = [](const SPW::Entity& e, SPW::KeyCode keycode)
	{
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
			mainCameraTrans->position += 0.01f * forward;
		if (keycode == SPW::Key::S)
			mainCameraTrans->position -= 0.01f * forward;
		if (keycode == SPW::Key::A)
			mainCameraTrans->position -= 0.01f * right;
		if (keycode == SPW::Key::D)
			mainCameraTrans->position += 0.01f * right;
		if (keycode == SPW::Key::Q)
			mainCameraTrans->position -= 0.01f * up;
		if (keycode == SPW::Key::E)
			mainCameraTrans->position += 0.01f * up;
	};
	auto mouse = camera->emplace<SPW::MouseComponent>();
	mouse->cursorMovementCallBack = [](const SPW::Entity& e, double x_pos, double y_pos, double x_pos_bias,
	                                   double y_pos_bias)
	{
		auto transform = e.component<SPW::TransformComponent>();
		transform->rotation.x -= y_pos_bias * 0.02;
		transform->rotation.y -= x_pos_bias * 0.1;
	};
	cameraKey->onKeyHeldCallBack = cb;
	cameraKey->onKeyDownCallBack = cb;
	return camera->component<SPW::IDComponent>()->getID();
}

const SPW::UUID& CreateCamera2(const std::shared_ptr<SPW::Scene>& scene, float width, float height)
{
	// add a camera entity
	auto camera = scene->createEntity("not main camera");
	camera->emplace<SPW::AudioListener>();
	auto mainCameraTrans = camera->emplace<SPW::TransformComponent>();
	mainCameraTrans->position = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	auto cam = camera->emplace<SPW::CameraComponent>(SPW::PerspectiveType);
	cam->fov = 60;
	cam->aspect = width / height;
	cam->near = 0.01;
	cam->far = 50;

	cam->whetherMainCam = false;
	//add a key component for testing, press R to rotate
	auto cameraKey = camera->emplace<SPW::KeyComponent>();
	auto cb = [](const SPW::Entity& e, SPW::KeyCode keycode)
	{
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
			mainCameraTrans->position += 0.01f * forward;
		if (keycode == SPW::Key::S)
			mainCameraTrans->position -= 0.01f * forward;
		if (keycode == SPW::Key::A)
			mainCameraTrans->position -= 0.01f * right;
		if (keycode == SPW::Key::D)
			mainCameraTrans->position += 0.01f * right;
		if (keycode == SPW::Key::Q)
			mainCameraTrans->position -= 0.01f * up;
		if (keycode == SPW::Key::E)
			mainCameraTrans->position += 0.01f * up;
	};
	// auto mouse = camera->emplace<SPW::MouseComponent>();
	// mouse->cursorMovementCallBack = [](const SPW::Entity& e, double x_pos, double y_pos, double x_pos_bias,
	//                                    double y_pos_bias)
	// {
	// 	auto transform = e.component<SPW::TransformComponent>();
	// 	transform->rotation.x -= y_pos_bias * 0.02;
	// 	transform->rotation.y -= x_pos_bias * 0.1;
	// };
	cameraKey->onKeyHeldCallBack = cb;
	cameraKey->onKeyDownCallBack = cb;
	return camera->component<SPW::IDComponent>()->getID();
}

std::shared_ptr<SPW::Entity> createPlight(const std::shared_ptr<SPW::Scene>& scene, glm::vec3 position, glm::vec3 color)
{
	auto light = scene->createEntity("light");
	auto lightTrans = light->emplace<SPW::TransformComponent>();
	auto lightCom = light->emplace<SPW::PointLightComponent>();
	lightCom->ambient = color;
	lightTrans->position = position;
	return light;
}

std::shared_ptr<SPW::Entity> createDirectionalLight(const std::shared_ptr<SPW::Scene>& scene, glm::vec3 rotation, glm::vec3 color)
{
	auto light = scene->createEntity("light");
	auto lightTrans = light->emplace<SPW::TransformComponent>();
	auto lightCom = light->emplace<SPW::DirectionalLightComponent>();
	lightCom->ambient = color;
	lightTrans->rotation = rotation;
	return light;
}

struct NoOpDeleter
{
	template <typename T>
	void operator()(T*) const noexcept
	{
	}
};

// test usage
class Transformer : public SPW::WindowEventResponder
{
public:
	explicit Transformer(const std::shared_ptr<SPW::EventResponderI>& parent)
		: SPW::WindowEventResponder(parent)
	{
	}

	bool onWindowResize(int w, int h) override
	{
		std::cout << "window resize" << "(" << w << ", " << h << ")" << std::endl;
		bool should_update = false;
		if (w < 500)
		{
			w = 500;
			should_update = true;
		}
		if (h < 400)
		{
			h = 400;
			should_update = true;
		}
		if (should_update && !window.expired())
			window.lock()->setSize(w, h);
		// set projection
		// TODO: add a responder to each camera
		scene.lock()->forEach([=](SPW::CameraComponent* cam) {
		                      cam->aspect = float(w) / float(h);
		                      if (cam->getType() == SPW::UIOrthoType) {
		                      cam->right = w;
		                      cam->top = h;
		                      }
		                      }, SPW::CameraComponent);
		return true;
	}

	// windows size is not equal to frame buffer size
	bool onFrameResize(int w, int h) override
	{
		glViewport(0, 0, w, h);
		std::cout << "frame resize" << "(" << w << ", " << h << ")" << std::endl;
		// set projection
		return false;
	}

	const char* getName() final
	{
		return "Transformer";
	}

	std::weak_ptr<SPW::WindowI> window;
	std::weak_ptr<SPW::Scene> scene;
};

// #define SAVE_SKYBOX
#define LOAD_ASSET

class SPWTestApp : public SPW::AppDelegateI
{
public:
	explicit SPWTestApp(std::shared_ptr<SPW::EventResponderI>& app, const char* name)
		: SPW::AppDelegateI(app), _name(name)
	{
	}

	void onAppInit() final
	{
		// 2. Simulate a process of loading some resources into a scene
#ifdef LOAD_ASSET
		{
			auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + "mantis/mantis.json");
			SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
			// [data.assetID] = std::move(data);
		}

		{
			auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + "companion_cube/companion_cube.json");
			SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
		}

		{
			auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + "sand_cube/sand_cube.json");
			SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
		}


		{
			auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + "dragon/dragon.json");
			SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
		}


		{
			auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + "skybox/skybox.json");
			SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
		}

#endif

#ifdef SAVE_SKYBOX
		std::unique_ptr<SPW::AssetData>  data = std::make_unique<SPW::AssetData>();
		data->assetID = SPW::FileSystem::GenerateRandomUUID();
		data->assetName = "skybox";
		data->meshes.emplace_back(SPW::createSkyBoxMesh());
		data->meshURI = SPW::FileSystem::GenerateRandomUUID();
		data->meshes[0].materialID = data->meshURI;
		SPW::MaterialData material;
		material.ID = data->meshURI;
		auto id_0 = SPW::FileSystem::GenerateRandomUUID();
		auto id_1 = SPW::FileSystem::GenerateRandomUUID();
		auto id_2 = SPW::FileSystem::GenerateRandomUUID();
		auto id_3 = SPW::FileSystem::GenerateRandomUUID();
		auto id_4 = SPW::FileSystem::GenerateRandomUUID();
		auto id_5 = SPW::FileSystem::GenerateRandomUUID();
		material.m_TextureIDMap =
		{
			{SPW::TextureMapType::Albedo, id_0},
			{SPW::TextureMapType::Normal, id_1},
			{SPW::TextureMapType::Metalness, id_2},
			{SPW::TextureMapType::Roughness, id_3},
			{SPW::TextureMapType::AmbientOcclusion, id_4},
			{SPW::TextureMapType::Unknown, id_5},
		};

		data->materials.emplace_back(material);
		data->textures =
		{
			{id_0, "./resources/texture/skybox/right.jpg"},
			{id_1, "./resources/texture/skybox/left.jpg"},
			{id_2, "./resources/texture/skybox/top.jpg"},
			{id_3, "./resources/texture/skybox/bottom.jpg"},
			{id_4, "./resources/texture/skybox/front.jpg"},
			{id_5, "./resources/texture/skybox/back.jpg"},
		};
		SPW::AssetManager::SaveAsset(std::move(data), "");
#endif

		// -------------------------------OFFLINE TEST-------------------------------------------

		std::shared_ptr<SPW::GlfwWindow> window = std::make_shared<SPW::GlfwWindow>();
		app->window = window;
		app->window->setSize(1280, 720);
		app->window->setTitle("Editor Test");

		// weak strong dance
		std::weak_ptr weak_window = window;
		window->onWindowCreated([weak_window, this](GLFWwindow* handle)
		{
			if (weak_window.expired())
			{
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

			auto ptr = std::shared_ptr<EventResponderI>(app);
			m_ImguiManager = std::make_shared<SPW::ImGuiManager>(handle, scene, ptr);

			bool isEditor = !(!m_ImguiManager);
			// create render system
			auto rendersystem = std::make_shared<SPW::SPWRenderSystem>(scene, renderBackEnd,
			                                                           weak_window.lock()->frameWidth(),
			                                                           weak_window.lock()->frameHeight(), isEditor);
			// add system
			scene->m_renderSystem = rendersystem;
			scene->addSystem(std::make_shared<SPW::AudioSystem>(scene));
			scene->addSystem(rendersystem);
			scene->addSystem(std::make_shared<SPW::KeyControlSystem>(scene));
			scene->addSystem(std::make_shared<SPW::MouseControlSystem>(scene));
			scene->addSystem(std::make_shared<SPW::AnimationSystem>(scene));
			scene->addSystem(std::make_shared<SPW::PhysicSystem>(scene));

			// create ui camera
			scene->uiCamera = scene->createEntity("ui_camera");
			auto ui_trans = scene->uiCamera->emplace<SPW::TransformComponent>();
			ui_trans->position = glm::vec3(0, 0, -1);
			auto ui_camera = scene->uiCamera->emplace<SPW::CameraComponent>(SPW::UIOrthoType);
			ui_camera->left = 0;
			ui_camera->right = weak_window.lock()->frameWidth();
			ui_camera->bottom = 0;
			ui_camera->top = weak_window.lock()->frameHeight();

			// ------ create main render graph ----------------
			SPW::RenderGraphManager::getInstance()->createRenderGraph(renderBackEnd, SPW::kDefferShadingGraph);
			SPW::RenderGraphManager::getInstance()->createRenderGraph(renderBackEnd, SPW::kSkyBoxRenderGraph);

			SPW::RenderGraphManager::getInstance()->forEachShader([&rendersystem](const SPW::ShaderDesc& shader) {
				rendersystem->addShaderDescriptor(shader);
				});

			// SPW::RenderGraphManager::getInstance()->AddSkyboxGraph(rendersystem->skyBoxGraph, rendersystem->skyBoxNode);

			// --------------- create shader ---------------

			auto camera_id = CreateACamera(scene, weak_window.lock()->width(), weak_window.lock()->height());
			auto camera2 = CreateCamera2(scene, weak_window.lock()->width(), weak_window.lock()->height());

			// -- AUDIO ----------------------------------------------------------------------------
			auto audioClip = scene->createEntity("audio");
			audioClip->emplace<SPW::TransformComponent>();
			//= scene->createEntity("audio");
			std::string flyMeToTheMoon = SPW::Config::k_WorkingProjectSounds + "FlyMeToTheMoon.mp3";
			std::string edm = SPW::Config::k_WorkingProjectSounds + "EDM.wav";
			auto audioCom = audioClip->emplace<SPW::AudioComponent>(std::vector{ flyMeToTheMoon, edm });
			audioCom->setState(flyMeToTheMoon, SPW::SoundState::Stop);
			audioCom->setState(edm, SPW::SoundState::Stop);

			// audioClip.swap();
			// --------------------------------------------------------------------------------
			SPW::ResourceManager::getInstance()->m_CameraIDMap["main"] = camera_id;
			SPW::ResourceManager::getInstance()->m_CameraIDMap["not main"] = camera2;
			SPW::ResourceManager::getInstance()->activeCameraID = camera_id;

			// --------------- dragon ---------------
			auto dragon = scene->createEntity("anim dragon");
			// transform
			auto dragon_transform = dragon->emplace<SPW::TransformComponent>();
			dragon_transform->scale = {0.05, 0.05, 0.05};
			dragon_transform->rotation = {0, 90, 0};
			dragon_transform->position = {0, -0.3, 0};

			// mesh filter
			auto dragon_model = dragon->emplace<SPW::MeshComponent>(camera_id);
			dragon_model->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].assetName;
			dragon_model->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].assetID;
			dragon_model->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].path;

			// mesh renderer
			dragon_model->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
			dragon_model->modelSubPassPrograms[GET_RENDER_NODE(SPW::kPointShadowNode)] = GET_SHADER_DESC(SPW::kAniPointShadowShader).uuid;
			dragon_model->modelSubPassPrograms[GET_RENDER_NODE(SPW::kDirectionalShadowNode)] = GET_SHADER_DESC(SPW::kAniDirectionalShadowShader).uuid;
			dragon_model->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(SPW::kAniGBufferShader).uuid;

			// animation 
			auto dragon_anim = dragon->emplace<SPW::AnimationComponent>( SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].assetName);
			dragon_anim->swapCurrentAnim("dragon_idle");
			// --------------------------------------------------------------------------------


			auto mantis = scene->createEntity("mantis");
			auto transform = mantis->emplace<SPW::TransformComponent>();
			transform->scale = {0.1, 0.1, 0.1};
			transform->rotation = {0, 90, 0};
			transform->position = {0, -0.3, 0};

			// add a model to show
			auto mantis_mesh = mantis->emplace<SPW::MeshComponent>(camera_id);
			mantis_mesh->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["mantis"].assetName;
			mantis_mesh->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["mantis"].assetID;
			mantis_mesh->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["mantis"].path;

			mantis_mesh->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
			mantis_mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kPointShadowNode)] = GET_SHADER_DESC(SPW::kPointShadowShader).uuid;
			mantis_mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kDirectionalShadowNode)] = GET_SHADER_DESC(SPW::kDirectionalShadowShader).uuid;
			mantis_mesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(SPW::kGBufferShader).uuid;

			auto  rigid1 = mantis->emplace<SPW::RigidDynamicComponent>();
			rigid1->rigidState = SPW::Awake;

			auto  collider1 = mantis->emplace<SPW::CapsuleCollider>();
			collider1->capsule_half_height_ = 0.1;
			collider1->capsule_radius_ = 0.1;
			collider1->degree = PxHalfPi;
			collider1->capsule_axis_ = glm::vec3(0, 1, 0);
			collider1->state = SPW::needAwake;
			collider1->is_trigger_ = false;

			// --------------------------------------------------------------------------------
			auto cubeObj = scene->createEntity("floor");
			auto cubeTrans = cubeObj->emplace<SPW::TransformComponent>();
			cubeTrans->scale = {5.0, 0.05, 5.0};
			cubeTrans->position.y -= 0.35f;
			auto cubemodel = cubeObj->emplace<SPW::MeshComponent>(camera_id);
			// SPW::ResourceManager::getInstance()->m_AssetDataMap["sand_cube"].assetName);
			cubemodel->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["sand_cube"].assetID;
			cubemodel->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["sand_cube"].assetName;
			cubemodel->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["sand_cube"].path;

			cubemodel->bindRenderGraph = GET_RENDER_GRAPH(SPW::kDefferShadingGraph);
			cubemodel->modelSubPassPrograms[GET_RENDER_NODE(SPW::kGBufferNode)] = GET_SHADER_DESC(SPW::kFloorGBufferShader).uuid;

			auto rigid2 = cubeObj->emplace<SPW::RigidStaticComponent>();
			rigid2->rigidState = SPW::Awake;
			auto collider2 = cubeObj->emplace<SPW::BoxCollider>();
			collider2->box_size_ = glm::vec3(10, 0.001, 10);
			collider2->state = SPW::needAwake;

			// ------ create render graph for skybox ----------------

			auto skybox = scene->createEntity("skybox");
			auto skyboxTrans = skybox->emplace<SPW::TransformComponent>();
			auto skyMesh = skybox->emplace<SPW::MeshComponent>(camera_id);
			skyMesh->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["skybox"].assetID;
			skyMesh->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["skybox"].assetName;
			skyMesh->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["skybox"].path;

			skyMesh->bindRenderGraph = GET_RENDER_GRAPH(SPW::kSkyBoxRenderGraph);
			skyMesh->modelSubPassPrograms[GET_RENDER_NODE(SPW::kSkyboxNode)] = GET_SHADER_DESC(SPW::kSkyBoxShader).uuid;


			auto light1 = createPlight(scene, {1, 1, 0}, {1, 0.5, 0});
			auto light2 = createPlight(scene, {-1, 1, 0}, {0, 0.5, 1});
			auto light3 = createDirectionalLight(scene, {30, 60, 0}, {0.5, 0, 1});
			auto light4 = createDirectionalLight(scene, {30, -60, 0}, {0.5, 1, 0});
			static int control_id = 0;
			auto light_controller = [](int idx)
			{
				return [idx](const SPW::Entity& en, SPW::KeyCode code)
				{
					if (code == SPW::KeyCode::D1)
					{
						control_id = 0;
					}
					else if (code == SPW::KeyCode::D2)
					{
						control_id = 1;
					}
					else if (code == SPW::KeyCode::D3)
					{
						control_id = 2;
					}
					else if (code == SPW::KeyCode::D4)
					{
						control_id = 3;
					}
					if (control_id == idx)
					{
						if ((control_id == 1 || control_id == 0))
						{
							if (code == SPW::KeyCode::Up)
							{
								en.component<SPW::TransformComponent>()->position.z -= 0.1;
							}
							else if (code == SPW::KeyCode::Down)
							{
								en.component<SPW::TransformComponent>()->position.z += 0.1;
							}
							else if (code == SPW::KeyCode::Left)
							{
								en.component<SPW::TransformComponent>()->position.x += 0.1;
							}
							else if (code == SPW::KeyCode::Right)
							{
								en.component<SPW::TransformComponent>()->position.x -= 0.1;
							}
						}
						else
						{
							if (code == SPW::KeyCode::Up)
							{
								en.component<SPW::TransformComponent>()->rotation.x -= 5;
							}
							else if (code == SPW::KeyCode::Down)
							{
								en.component<SPW::TransformComponent>()->rotation.x += 5;
							}
							else if (code == SPW::KeyCode::Left)
							{
								en.component<SPW::TransformComponent>()->rotation.y += 5;
							}
							else if (code == SPW::KeyCode::Right)
							{
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


			TEST_LOGGER_INFO("ImGui : {}", IMGUI_VERSION)
#ifdef IMGUI_HAS_VIEWPORT
			TEST_LOGGER_INFO("ImGui : + viewport")
#endif
#ifdef IMGUI_HAS_DOCK
			TEST_LOGGER_INFO("ImGui : + docking")
#endif

			// init scene
			scene->initial();
		});
	}

	void beforeAppUpdate() final
	{
		scene->beforeUpdate();
	}

	void onAppUpdate(const SPW::TimeDuration& du) final
	{
		// physics, computation
		scene->onUpdate(du);
	}

	void afterAppUpdate() final
	{
		scene->afterUpdate();
		// ----------------------------------------------------------------------------------------
		m_ImguiManager->Render();
	}

	void onUnConsumedEvents(std::vector<std::shared_ptr<SPW::EventI>>& events) final
	{
		// for (auto &e : events) {
		//     DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
		// }
	}

	void onAppStopped() final
	{
		sol::state state;
		state.open_libraries(sol::lib::base, sol::lib::package);
		std::string x = state["package"]["path"];
		state["package"]["path"] = x + ";./resources/scripts/lua/?.lua";
		try
		{
			if (state.script_file("./resources/scripts/lua/test.lua").valid())
			{
				sol::protected_function main_function = state["main"];
				sol::protected_function_result result = main_function();
				if (!result.valid())
				{
					std::cout << "execution error" << std::endl;
				}
			}
		}
		catch (sol::error& e)
		{
			std::cout << e.what() << std::endl;
		}
		std::cout << "app stopped" << std::endl;
		scene->onStop();
	}

	void solveEvent(const std::shared_ptr<SPW::EventI>& e) final
	{
		e->dispatch<SPW::WindowCloseType, SPW::WindowEvent>(
			[this](SPW::WindowEvent* e)
			{
				// close application

				app->stop();
				return true;
			});
		SPW::EventResponderI::solveEvent(e);
	}

	const char* getName() final { return _name; }
	const char* _name;
	std::shared_ptr<SimpleRender> render;
	std::shared_ptr<SPW::Scene> scene;
	std::shared_ptr<SPW::RenderBackEndI> renderBackEnd;
	std::shared_ptr<SPW::ImGuiManager> m_ImguiManager;
};

// main entrance
int main(int argc, char** argv)
{
	if (SPW::ConfigManager::Boost())
	{
		TEST_LOGGER_INFO("Boot Edtior Test")
	}
	else return -1;


	// app test
	auto appProxy =
		SPW::Application::create<SPWTestApp>("SPWTestApp");
	return appProxy->app->run(argc, argv);
}

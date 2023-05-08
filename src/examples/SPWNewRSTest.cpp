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
#include "EcsFramework/Component/PhysicalComponent/Collider.h"

#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/NewRenderSystem/DefaultRenderPass.hpp"
#include "EcsFramework/System/PhysicSystem/PhysicSystem.h"

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
#include "ImGui/ImGuiManager.hpp"
#include "Asset/Serializer/EntitySerializer.h"
#include "EcsFramework/Component/AnimationComponent/AnimationComponent.h"
#include "EcsFramework/System/AnimationSystem/AnimationSystem.h"
#include "IO/ConfigManager.h"

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
		transform->rotation.x -= y_pos_bias * 0.1;
		transform->rotation.y -= x_pos_bias * 0.1;
	};
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

std::shared_ptr<SPW::Entity> createDlight(const std::shared_ptr<SPW::Scene>& scene, glm::vec3 rotation, glm::vec3 color)
{
	auto light = scene->createEntity("light");
	auto lightTrans = light->emplace<SPW::TransformComponent>();
	auto lightCom = light->emplace<SPW::DirectionalLightComponent>();
	lightCom->ambient = color;
	lightTrans->rotation = rotation;
	return light;
}

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
// #define LOAD_ASSET

class SPWTestApp : public SPW::AppDelegateI
{
public:
	explicit SPWTestApp(std::shared_ptr<SPW::EventResponderI>& app, const char* name)
		: SPW::AppDelegateI(app), _name(name)
	{
	}

	void onAppInit() final
	{
		// TomlWriter();

		// -------------------------------OFFLINE TEST-------------------------------------------

		// 1. Simulate a process of an engine boost
		SPW::FileSystem::Boost();
#ifdef SAVE_SKYBOX
        std::unique_ptr<SPW::AssetData> data = std::make_unique<SPW::AssetData>();
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
        SPW::AssetManager::SaveAsset(std::move(data),  "");
#endif
		// 2. Simulate a process of loading some resources into a scene
		{
			auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + "mantis/mantis.json");
			SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
			// [data.assetID] = std::move(data);
		}

		{
			auto data = SPW::AssetManager::LoadAsset(
				SPW::Config::k_WorkingProjectAssets + "companion_cube/companion_cube.json");
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

		// -------------------------------OFFLINE TEST-------------------------------------------

		std::shared_ptr<SPW::GlfwWindow> window = std::make_shared<SPW::GlfwWindow>();
		app->window = window;
		app->window->setSize(1280, 720);
		app->window->setTitle("Editor Test");

		transformer = std::make_shared<Transformer>(app->delegate.lock());
		transformer->window = window;

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

			// create render system
			auto rendersystem = std::make_shared<SPW::SPWRenderSystem>(scene, renderBackEnd,
			                                                           weak_window.lock()->frameWidth(),
			                                                           weak_window.lock()->frameHeight());
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
			ui_camera->right = float(weak_window.lock()->frameWidth());
			ui_camera->bottom = 0;
			ui_camera->top = float(weak_window.lock()->frameHeight());

            // ------ create main render graph ----------------

            auto defferShadering = rendersystem->createRenderGraph();

            auto p_shadowmap_node = defferShadering->createRenderNode<SPW::ModelRepeatPassNode>(SPW::CubeMapType, SPW::RepeatForPLights, 10);
            p_shadowmap_node->width = 256;
            p_shadowmap_node->height = 256;
            p_shadowmap_node->clearType = SPW::ClearDepth;

            auto d_shadowmap_node = defferShadering->createRenderNode<SPW::ModelRepeatPassNode>(SPW::ColorType, SPW::RepeatForDLights, 10);
            d_shadowmap_node->width = 4096;
            d_shadowmap_node->height = 4096;
            d_shadowmap_node->clearType = SPW::ClearDepth;

            auto p_shadowmap_output = p_shadowmap_node->addAttachment(SPW::Depth);
            auto d_shadowmap_output = d_shadowmap_node->addAttachment(SPW::Depth);

            auto defferNode = defferShadering->createRenderNode<SPW::ModelPassNode>(SPW::ColorType);
            defferNode->width = weak_window.lock()->frameWidth();
            defferNode->height = weak_window.lock()->frameHeight();
            defferNode->clearType = SPW::ClearType::ClearAll;
            defferNode->depthTest = true;
            defferNode->depthCompType = SPW::DepthCompType::LESS_Type;
            auto gPosition = defferNode->addAttachment(SPW::ColorAttachmentFormat::RGBA32);
            auto gNormal = defferNode->addAttachment(SPW::ColorAttachmentFormat::RGB16);
            auto gAlbedo = defferNode->addAttachment(SPW::ColorAttachmentFormat::RGBA32);
            auto gMetalRognessAO = defferNode->addAttachment(SPW::ColorAttachmentFormat::RGBA32);
            auto gDepth = defferNode->addAttachment(SPW::ColorAttachmentFormat::Depth);

            auto pbr_deffer_shading_desc = SPW::defferPBR(p_shadowmap_output, d_shadowmap_output, gPosition, gNormal, gAlbedo, gMetalRognessAO, gDepth, {
                "defferPBR",
                "./resources/shaders/pbr_defer_shading.vert",
                "./resources/shaders/pbr_defer_shading.frag"
            });

            auto GBufferShading = defferShadering->createRenderNode<SPW::ScreenPassNode>(pbr_deffer_shading_desc);
            GBufferShading->bindInputPort(gPosition);
            GBufferShading->bindInputPort(gNormal);
            GBufferShading->bindInputPort(gAlbedo);
            GBufferShading->bindInputPort(gMetalRognessAO);
            GBufferShading->bindInputPort(p_shadowmap_output);
            GBufferShading->bindInputPort(d_shadowmap_output);
            GBufferShading->bindInputPort(gDepth);
            GBufferShading->depthCompType = SPW::DepthCompType::LESS_Type;

            SPW::AttachmentPort screen_color_port = GBufferShading->addScreenAttachment(SPW::ScreenColorType);

            auto SSR_desc = SPW::SSR(gMetalRognessAO,gDepth,gNormal,gPosition,screen_color_port);
            auto SSR_node = defferShadering->createRenderNode<SPW::ImagePassNode>(SSR_desc);
            SSR_node->clearType = SPW::ClearColor;
            SSR_node->depthTest = false;

            SSR_node->bindInputPort(gPosition);
            SSR_node->bindInputPort(gNormal);
            SSR_node->bindInputPort(gMetalRognessAO);
            SSR_node->bindInputPort(gDepth);
            SSR_node->bindInputPort(screen_color_port);
            auto reflect_port = SSR_node->addAttachment(SPW::RGBA32);

            auto SSR_blur_desc = SPW::SSR_blur(reflect_port,gDepth,screen_color_port);
            auto SSR_blur_node = defferShadering->createRenderNode<SPW::ScreenPassNode>(SSR_blur_desc);
            SSR_blur_node->clearType = SPW::ClearColor;
            SSR_blur_node->depthTest = false;

            SSR_blur_node->bindInputPort(reflect_port);
            SSR_blur_node->bindInputPort(gDepth);
            SSR_blur_node->bindInputPort(screen_color_port);

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
            SPW::ShaderHandle GBuffer({
                                              "drawGBuffer",
                                              "./resources/shaders/GBuffer.vert",
                                              "./resources/shaders/GBuffer.frag"
                                      });
            SPW::ShaderHandle GBuffer_floor({
                                                    "drawFloorGBuffer",
                                                    "./resources/shaders/GBuffer.vert",
                                                    "./resources/shaders/GBuffer_floor.frag"
                                            });
            SPW::ShaderHandle ani_GBuffer({
                                                  "draw_ani_GBuffer",
                                                  "./resources/shaders/ani_GBuffer.vert",
                                                  "./resources/shaders/GBuffer.frag"
                                          });
            auto p_shadow_desc = SPW::P_shadowmap_desc();
            auto d_shadow_desc = SPW::D_shadowmap_desc();
            auto p_ani_shadow_desc = SPW::P_ani_shadowmap_desc();
            auto d_ani_shadow_desc = SPW::D_ani_shadowmap_desc();

            auto pbr_ani_light_shadow_desc = PBR_ani_shadow_desc(p_shadowmap_output, d_shadowmap_output, pbr_ani_light_shadow);
            auto pbr_light_shadow_desc = PBR_light_with_shadow_desc(p_shadowmap_output, d_shadowmap_output, pbr_light_shadow);
            auto pbr_light_shadow_tiled_desc = PBR_light_with_shadow_desc(p_shadowmap_output, d_shadowmap_output, pbr_light_shadow_tiled);
            auto GBuffer_desc = SPW::GBuffer_desc(GBuffer);
            auto GBuffer_floor_desc = SPW::GBuffer_desc(GBuffer_floor);
            auto ani_GBuffer_desc = SPW::ani_GBuffer_desc(ani_GBuffer);

            auto skybox_desc = SPW::SkyBoxShader_desc();
            rendersystem->addShaderDescriptor(pbr_light_shadow_desc);
            rendersystem->addShaderDescriptor(pbr_light_shadow_tiled_desc);
            rendersystem->addShaderDescriptor(p_shadow_desc);
            rendersystem->addShaderDescriptor(d_shadow_desc);
            rendersystem->addShaderDescriptor(skybox_desc);
            rendersystem->addShaderDescriptor(p_ani_shadow_desc);
            rendersystem->addShaderDescriptor(d_ani_shadow_desc);
            rendersystem->addShaderDescriptor(pbr_ani_light_shadow_desc);
            rendersystem->addShaderDescriptor(GBuffer_desc);
            rendersystem->addShaderDescriptor(ani_GBuffer_desc);
            rendersystem->addShaderDescriptor(pbr_deffer_shading_desc);
            rendersystem->addShaderDescriptor(GBuffer_floor_desc);
            rendersystem->addShaderDescriptor(SSR_desc);
            rendersystem->addShaderDescriptor(SSR_blur_desc);
			// --------------- create shader ---------------

			auto camera_id = CreateACamera(scene, weak_window.lock()->width(), weak_window.lock()->height());
			// --------------------------------------------------------------------------------
			SPW::ResourceManager::getInstance()->m_CameraIDMap["main"] = camera_id;
			SPW::ResourceManager::getInstance()->m_ShaderMap["p_shadow_desc"] = p_shadow_desc;
			SPW::ResourceManager::getInstance()->m_ShaderMap["d_shadow_desc"] = d_shadow_desc;
			SPW::ResourceManager::getInstance()->m_ShaderMap["pbr_light_shadow_desc"] = pbr_light_shadow_desc;
			SPW::ResourceManager::getInstance()->m_ModelRepeatPassNodes["p_shadowmap_node"] = p_shadowmap_node;
			SPW::ResourceManager::getInstance()->m_ModelRepeatPassNodes["d_shadowmap_node"] = d_shadowmap_node;

			// --------------- dragon ---------------
			auto dragon = scene->createEntity("anim dragon");
			auto dragon_transform = dragon->emplace<SPW::TransformComponent>();
			dragon_transform->scale = {1, 1, 1};
			dragon_transform->rotation = {0, 90, 0};
			dragon_transform->position = {0, 0, 0};

			auto dragon_model = dragon->emplace<SPW::MeshComponent>(camera_id);
			dragon_model->bindRenderGraph = defferShadering->graph_id;
			dragon_model->modelSubPassPrograms[p_shadowmap_node->pass_id] = p_ani_shadow_desc.uuid;
			dragon_model->modelSubPassPrograms[d_shadowmap_node->pass_id] = d_ani_shadow_desc.uuid;
			dragon_model->modelSubPassPrograms[defferNode->pass_id] = ani_GBuffer_desc.uuid;

			dragon_model->assetID   = SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].assetID;
			dragon_model->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].assetName;
			dragon_model->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].path;

            // add animation
			auto dragon_anim = dragon->emplace<SPW::AnimationComponent>( SPW::ResourceManager::getInstance()->m_AssetDataMap["dragon"].assetName );
			dragon_anim->swapCurrentAnim("dragon_idle");

			// --------------------------------------------------------------------------------
			auto mantis = scene->createEntity("mantis");
			auto transform = mantis->emplace<SPW::TransformComponent>();
			transform->scale = {0.1, 0.1, 0.1};
			transform->rotation = {0, 90, 0};
			transform->position = {5, 0, 0};

			// add a model to show
			auto mantis_mesh = mantis->emplace<SPW::MeshComponent>(camera_id);
			mantis_mesh->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["mantis"].assetName;
			mantis_mesh->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["mantis"].assetID;
			mantis_mesh->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["mantis"].path;

			mantis_mesh->bindRenderGraph = defferShadering->graph_id;
			mantis_mesh->modelSubPassPrograms[p_shadowmap_node->pass_id] = p_shadow_desc.uuid;
			mantis_mesh->modelSubPassPrograms[d_shadowmap_node->pass_id] = d_shadow_desc.uuid;
			mantis_mesh->modelSubPassPrograms[defferNode->pass_id] = GBuffer_desc.uuid;

			auto  rigid1 = mantis->emplace<SPW::RigidDynamicComponent>();
			rigid1->rigidState = SPW::Awake;

			auto  collider1 = mantis->emplace<SPW::CapsuleCollider>();
			collider1->capsule_half_height_ = 0.1;
			collider1->capsule_radius_ = 0.1;
			collider1->degree = PxHalfPi;
			collider1->capsule_axis_ = glm::vec3(0, 0, 1);
			collider1->state = SPW::needAwake;
			collider1->is_trigger_ = false;

			//  --------------------------------------------------------------------------------
			auto cubeObj = scene->createEntity("floor");
			auto cubeTrans = cubeObj->emplace<SPW::TransformComponent>();
			cubeTrans->scale = {50.0, 0.05, 50.0};
			cubeTrans->position.y -= 0.35f;
			auto cubemodel = cubeObj->emplace<SPW::MeshComponent>(camera_id);
			cubemodel->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["sand_cube"].assetID;
			cubemodel->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["sand_cube"].assetName;
			cubemodel->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["sand_cube"].path;

			cubemodel->bindRenderGraph = defferShadering->graph_id;
			cubemodel->modelSubPassPrograms[defferNode->pass_id] = GBuffer_desc.uuid;

			auto  rigid2 = cubeObj->emplace<SPW::RigidStaticComponent>();
			rigid2->rigidState = SPW::Awake;
			auto  collider2 = cubeObj->emplace<SPW::BoxCollider>();
			collider2->box_size_ = glm::vec3(50, 0.05, 50);
			collider2->state = SPW::needAwake;

			// --------------------------------------------------------------------------------
			auto skybox = scene->createEntity("skybox");
            skybox->emplace<SPW::TransformComponent>();
			auto skyMesh = skybox->emplace<SPW::MeshComponent>(camera_id);
			skyMesh->assetID = SPW::ResourceManager::getInstance()->m_AssetDataMap["skybox"].assetID;
			skyMesh->assetName = SPW::ResourceManager::getInstance()->m_AssetDataMap["skybox"].assetName;
			skyMesh->assetPath = SPW::ResourceManager::getInstance()->m_AssetDataMap["skybox"].path;

			skyMesh->bindRenderGraph = rendersystem->skyBoxGraph->graph_id;
			skyMesh->modelSubPassPrograms[rendersystem->skyBoxNode->pass_id] = skybox_desc.uuid;

			auto light1 = createPlight(scene, {1, 1, 0}, {1, 0.5, 0});
			auto light2 = createPlight(scene, {-1, 1, 0}, {0, 0.5, 1});
			auto light3 = createDlight(scene, {30, 60, 0}, {0.5, 0, 1});
			auto light4 = createDlight(scene, {30, -60, 0}, {0.5, 1, 0});
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

			std::cout << "ImGui" << IMGUI_VERSION << std::endl;
			// init scene
			scene->initial();
			transformer->scene = scene;
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
	}

	void onUnConsumedEvents(std::vector<std::shared_ptr<SPW::EventI>>& events) final
	{
		// for (auto &e : events) {
		//     DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
		// }
	}

	void onAppStopped() final
	{
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
		e->dispatch<SPW::MouseDownType, SPW::MouseEvent>(
			[this](SPW::MouseEvent* e)
			{
				static bool enabled = false;
				if (e->button_code == SPW::MouseCode::ButtonRight) {
					enabled = !enabled;
					app->window->enableCursor(enabled);
				}
				return true;
			}
		);
		SPW::EventResponderI::solveEvent(e);
	}

	const char* getName() final { return _name; }
	const char* _name;
	std::shared_ptr<Transformer> transformer;
	std::shared_ptr<SimpleRender> render;
	std::shared_ptr<SPW::Scene> scene;
	std::shared_ptr<SPW::RenderBackEndI> renderBackEnd;
};

// main entrance
int main(int argc, char** argv)
{
	if (SPW::ConfigManager::ReadConfig())
		std::cout << "Successfully read config file" << std::endl;

	SPW::FileSystem::MountFromConfig();

	// app test
	auto appProxy =
		SPW::Application::create<SPWTestApp>("SPWTestApp");
	return appProxy->app->run(argc, argv);
}

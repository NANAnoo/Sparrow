#include <iostream>

#include <memory>
#include <sol/sol.hpp>

#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Entity/Entity.hpp"
#include "Model/Mesh.h"
#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Control/KeyCodes.h"

#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"
#include "EcsFramework/System/NewRenderSystem/DefaultRenderPass.hpp"
#include "Model/Model.h"

#include "Utils/UUID.hpp"

#include "EcsFramework/System/AudioSystem/AudioSystem.h"

#include "Platforms/OPENGL/OpenGLBackEnd.h"
#include "Platforms/OPENGL/OpenGLxGLFWContext.hpp"

#include "SimpleRender.h"
#include "IO/ResourceManager.h"
#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>

#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/System/NewRenderSystem/SPWRenderSystem.h"
#include "IO/FileSystem.h"
#include "ImGui/ImGuiManager.hpp"

#include "Asset/Asset.hpp"
#include "Asset/AssetData/MeshData.h"
#include "Asset/AssetData/ModelData.h"
#include "Asset/AssetData/MaterialData.h"
#include "Asset/Serializer/EntitySerializer.h"
#include "Asset/ModelLoader/ModelLoader.h"

std::shared_ptr<SPW::Model> createModel() {
    //return SPW::ResourceManager::getInstance()->LoadModel("./resources/models/mona2/mona.fbx");
    return SPW::ResourceManager::getInstance()->LoadModel("./resources/models/mantis/scene.gltf");
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
            mainCameraTrans->position +=0.01f * forward;
        if(keycode == SPW::Key::S)
            mainCameraTrans->position -=0.01f * forward;
        if(keycode == SPW::Key::A)
            mainCameraTrans->position -=0.01f * right;
        if(keycode == SPW::Key::D)
            mainCameraTrans->position +=0.01f * right;
        if(keycode == SPW::Key::Q)
            mainCameraTrans->position -=0.01f * up;
        if(keycode == SPW::Key::E)
            mainCameraTrans->position +=0.01f * up;
    };
    auto mouse = camera->emplace<SPW::MouseComponent>();
    mouse->cursorMovementCallBack = [](const SPW::Entity& e, double x_pos, double y_pos, double x_pos_bias, double y_pos_bias){
        auto transform = e.component<SPW::TransformComponent>();
        transform->rotation.x -= y_pos_bias * 0.02;
        transform->rotation.y -= x_pos_bias * 0.1 ;
    };
    cameraKey->onKeyHeldCallBack = cb;
    cameraKey->onKeyDownCallBack = cb;
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

struct NoOpDeleter {
    template <typename T>
    void operator()(T*) const noexcept {}
};

// test usage
class Transformer : public SPW::WindowEventResponder
{
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

std::string SPW::FileRoots::k_Root   = "C:/Users/dudu/Desktop/UserProject/"; // TODO : change this to your own path
std::string SPW::FileRoots::k_Engine = k_Root + "Engine/";
std::string SPW::FileRoots::k_Assets = k_Root + "Assets/";
std::string SPW::FileRoots::k_Scenes = k_Root + "Scenes/";


// #define IMPORT_MODEL
#define LOAD_ASSET
/*
 * TODO HACK TEMOPORY DATA
 */
std::vector<SPW::MaterialData> g_newMaterials;
std::vector<SPW::Mesh> g_newMeshes;
std::string g_assetID;
std::string g_path;
std::string g_meshURI;
std::unordered_map<std::string, std::string> g_textures;

class SPWTestApp : public SPW::AppDelegateI
{
public:
    explicit SPWTestApp(std::shared_ptr<SPW::EventResponderI> &app, const char *name)
      : SPW::AppDelegateI(app), _name(name)
    {    }

    void onAppInit() final
    {

// -------------------------------OFFLINE TEST-------------------------------------------

        // 1. Simulate a process of an engine boost
        SPW::FileSystem::Boost();
        // 2. Simulate a process of loading some resources into a scene
#if defined(IMPORT_MODEL)
        // assume load two models into the scene
        std::unique_ptr<SPW::ModelData> model_0 = SPW::ModelLoader::LoadModel("C:/Users/dudu/Downloads/mosquito_in_amber/scene.gltf");

        std::string absolute_modelDir = SPW::FileSystem::JoinPaths(SPW::FileRoots::k_Assets, model_0->name);
    	SPW::FileSystem::CreateDirectory(absolute_modelDir);

        // Update Model Path
        model_0->path = SPW::FileSystem::JoinPaths(absolute_modelDir, model_0->name) + ".json";

        // write file with absolute file path
    	std::ofstream of_file(model_0->path); // TODO .asset, use json to view the file currently
        cereal::JSONOutputArchive ar(of_file);

    	// copy texture files into /Assets/Textures...
        // 1. Create Texture Directory
        std::string model_texture_dir = SPW::FileSystem::JoinPaths(absolute_modelDir , "Textures/");
        SPW::FileSystem::CreateDirectory(model_texture_dir);
        // 2. Loop Textures and Copy Files into Texture Directory
    	for (auto& [k, v] : model_0->textures)
        {
            std::string destinationFilePath(model_texture_dir + SPW::FileSystem::ToFsPath(v).filename().string());
            SPW::FileSystem::CopyFile(v, destinationFilePath);

            // 3. Update Texture Path To Relative
            v = SPW::FileSystem::ToRelativePath(destinationFilePath, SPW::FileRoots::k_Root);
        }

        ar(
            // cereal::make_nvp("assetType", model_0->type),
            cereal::make_nvp("assetID", model_0->assetID), 
            cereal::make_nvp("assetPath", SPW::FileSystem::ToRelativePath(model_0->path, SPW::FileRoots::k_Root)),

            // cereal::make_nvp("assetName", model_0->name),
            cereal::make_nvp("meshURI", model_0->meshURI),
            cereal::make_nvp("materials", model_0->materials),
            cereal::make_nvp("textures", model_0->textures)
        );

        {
              std::ofstream mesh_bin(SPW::FileSystem::JoinPaths(absolute_modelDir, model_0->meshURI) + ".mesh", std::ios::binary);
              cereal::BinaryOutputArchive archive(mesh_bin);
              archive(cereal::make_nvp(model_0->meshURI, model_0->meshes));
        }
#elif defined(LOAD_ASSET)

        std::ifstream is_Scene(SPW::k_Assets + "/scene/scene.json"); // TODO: Select the asset file to Load by GUI operations
        cereal::JSONInputArchive ar(is_Scene);

        ar(
            cereal::make_nvp("assetID", g_assetID),
            cereal::make_nvp("assetPath", g_path),
            cereal::make_nvp("meshURI", g_meshURI),
            cereal::make_nvp("materials", g_newMaterials),
            cereal::make_nvp("textures", g_textures)
        );

        {
            const auto& path = SPW::FilePath(SPW::FileSystem::ToAbsolutePath(g_path)).parent_path().string();

        	std::ifstream mesh_bin(path + "/" + g_meshURI + ".mesh", std::ios::binary);
            cereal::BinaryInputArchive ar(mesh_bin);
            ar(cereal::make_nvp(g_meshURI, g_newMeshes));
        }

    	// __debugbreak();
#endif
// -------------------------------OFFLINE TEST-------------------------------------------



        std::shared_ptr<SPW::GlfwWindow> window = std::make_shared<SPW::GlfwWindow>();
        app->window = window;
        app->window->setSize(1280, 720);
        app->window->setTitle("Editor Test");

        transformer = std::make_shared<Transformer>(app->delegate.lock());
        transformer->window = window;

        // weak strong dance
        std::weak_ptr weak_window = window;
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
            renderSystem = std::make_shared<SPW::SPWRenderSystem>(scene, renderBackEnd, weak_window.lock()->frameWidth(), weak_window.lock()->frameHeight());
            // add system
            scene->addSystem(std::make_shared<SPW::AudioSystem>(scene));
            scene->addSystem(renderSystem);
            scene->addSystem(std::make_shared<SPW::KeyControlSystem>(scene));

            //TODO
            //scene->addSystem(std::make_shared<SPW::MouseControlSystem>(scene));

            // ------ create main render graph ----------------
            auto pbr_with_PDshadow = renderSystem->createRenderGraph();

            auto p_shadowmap_node = pbr_with_PDshadow->createRenderNode<SPW::ModelRepeatPassNode>(SPW::CubeMapType, SPW::RepeatForPLights, 10);
            p_shadowmap_node->width = 256;
            p_shadowmap_node->height = 256;
            p_shadowmap_node->clearType = SPW::ClearDepth;


            auto d_shadowmap_node = pbr_with_PDshadow->createRenderNode<SPW::ModelRepeatPassNode>(SPW::ColorType, SPW::RepeatForDLights, 10);
            d_shadowmap_node->width = 2048;
            d_shadowmap_node->height = 2048;
            d_shadowmap_node->clearType = SPW::ClearDepth;

            auto pbr_shadow_lighting_node = pbr_with_PDshadow->createRenderNode<SPW::ModelToScreenNode>();

            auto p_shadowmap_output = p_shadowmap_node->addAttachment(SPW::Depth);
            auto d_shadowmap_output = d_shadowmap_node->addAttachment(SPW::Depth);

            pbr_shadow_lighting_node->bindInputPort(p_shadowmap_output);
            pbr_shadow_lighting_node->bindInputPort(d_shadowmap_output);

            auto pbr_shadow_lighting_output = pbr_shadow_lighting_node->addScreenAttachment(SPW::ScreenColorType);
            // ------ create main render graph ----------------

            // ------ create post processing graph --------------
            auto post_processing_pass = renderSystem->createRenderGraph();
            SPW::AttachmentPort screen_color_port = {SPW::SCREEN_PORT, SPW::ScreenColorType};
            auto fxaa_node = post_processing_pass->createRenderNode<SPW::ScreenPassNode>(FXAA_desc(screen_color_port));
            fxaa_node->bindInputPort(screen_color_port);
            fxaa_node->depthTest = false;
            // ------ create post processing graph --------------

            // --------------- create shader ---------------
            SPW::ShaderHandle pbr_light_shadow({
                                                       "pbr_light_shadow",
                                                       "./resources/shaders/simpleVs.vert",
                                                       "./resources/shaders/pbrShadow.frag"
                                               });

            SPW::ShaderHandle pbr_light_shadow_tiled({
                                                             "pbr_light_shadow_tiled",
                                                             "./resources/shaders/simpleVs.vert",
                                                             "./resources/shaders/pbrShadowTiled.frag"
                                                     });
            auto p_shadow_desc = SPW::P_shadowmap_desc();
            auto d_shadow_desc = SPW::D_shadowmap_desc();

            auto pbr_light_shadow_desc = PBR_light_with_shadow_desc(p_shadowmap_output, d_shadowmap_output, pbr_light_shadow);
            auto pbr_light_shadow_tiled_desc = PBR_light_with_shadow_desc(p_shadowmap_output, d_shadowmap_output, pbr_light_shadow_tiled);
            renderSystem->addShaderDesciptor(pbr_light_shadow_desc);
            renderSystem->addShaderDesciptor(pbr_light_shadow_tiled_desc);
            renderSystem->addShaderDesciptor(p_shadow_desc);
            renderSystem->addShaderDesciptor(d_shadow_desc);

            // --------------- create shader ---------------
            auto camera_id = createMaincamera(scene, weak_window.lock()->width(), weak_window.lock()->height());
            // --------------------------------------------------------------------------------

            auto obj = scene->createEntity("test");
            auto transform = obj->emplace<SPW::TransformComponent>();
            transform->scale = {0.1, 0.1, 0.1};
            transform->rotation = {0, 90, 0};
            transform->position = {0, -0.3, 0};

            // add a model to show
            auto model = obj->emplace<SPW::MeshComponent>(camera_id);

            model->bindRenderGraph = pbr_with_PDshadow->graph_id;
            model->modelSubPassPrograms[p_shadowmap_node->pass_id] = p_shadow_desc.uuid;
            model->modelSubPassPrograms[d_shadowmap_node->pass_id] = d_shadow_desc.uuid;
            model->modelSubPassPrograms[pbr_shadow_lighting_node->pass_id] = pbr_light_shadow_desc.uuid;

            model->model = createModel();
/*
 * TODO HACK FOR SER TEST
 */
model->b_Asset   = true;
model->assetID   = g_assetID;
model->assetPath = g_path;
model->meshes    = g_newMeshes;
model->materials = g_newMaterials;
model->meshURI   = g_meshURI;
model->textures  = g_textures;
__debugbreak();

            // --------------------------------------------------------------------------------
            auto cubeObj = scene->createEntity("floor");
            auto cubeTrans = cubeObj->emplace<SPW::TransformComponent>();
            cubeTrans->scale = {5.0, 0.05, 5.0};
            cubeTrans->position.y-=0.35f;
            auto cubemodel = cubeObj->emplace<SPW::MeshComponent>(camera_id);
            cubemodel->model = createCubeModel();

            cubemodel->bindRenderGraph = pbr_with_PDshadow->graph_id;
            cubemodel->modelSubPassPrograms[pbr_shadow_lighting_node->pass_id] = pbr_light_shadow_tiled_desc.uuid;

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

            m_ImguiManager = std::make_shared<SPW::ImGuiManager>();
            m_ImguiManager->Init(handle);

  std::cout << "ImGui" << IMGUI_VERSION << std::endl;
#ifdef IMGUI_HAS_VIEWPORT
  std::cout << " +viewport";
#endif
#ifdef IMGUI_HAS_DOCK
  std::cout << " +docking"<< std::endl;
#endif

            // init scene
            scene->initial();
            transformer->scene = scene;

             // SPW::EntitySerializer::SaveScene(scene, "C:/Users/dudu/Desktop/");
        });
    }
    void beforeAppUpdate() final
	{
        scene->beforeUpdate();

    }
    void onAppUpdate(const SPW::TimeDuration &du) final{

        // physics, computation
        scene->onUpdate(du);
    }

    void afterAppUpdate() final{
        scene->afterUpdate();

//    	m_ImguiManager->RenderUIComponent<SPW::UIComponentType::Dockspace>("Right Dock Space");
//      m_ImguiManager->RenderUIComponent<SPW::UIComponentType::Dockspace>("Bottom Dock Space");
//      m_ImguiManager->RenderUIComponent<SPW::UIComponentType::Dockspace>("Left Dock Space");
       // m_ImguiManager->RenderUIComponent<SPW::UIComponentType::Dockspace>();
        m_ImguiManager->Begin();
        std::vector <std::shared_ptr<SPW::ImGuiPanel>> dockspacePanels;
        dockspacePanels.emplace_back(m_ImguiManager->m_ObjectPanel);
        dockspacePanels.emplace_back(m_ImguiManager->m_HierarchyPanel);
        dockspacePanels.emplace_back(m_ImguiManager->m_MainMenuBar);
        dockspacePanels.emplace_back(m_ImguiManager->m_FileExplorer);

        //render image
        GLuint texture_id = renderSystem->getTextureID();
        if(texture_id)
        {
            m_ImguiManager->CreateImagePanel(texture_id);
            dockspacePanels.emplace_back(m_ImguiManager->m_ImagePanel);
		}

        m_ImguiManager->CreateAndRenderDockspacePanel(dockspacePanels);

        
        //m_ImguiManager->RenderDockspacePanel();

        //----------------------------------------------------------------------------------------

        //m_ImguiManager->RenderUIComponent<SPW::UIComponentType::MenuBar>();
        //m_ImguiManager->RenderUIComponent<SPW::UIComponentType::ObjectPanel>();
        //m_ImguiManager->RenderUIComponent<SPW::UIComponentType::HierarchyPanel>();
        //m_ImguiManager->RenderUIComponent<SPW::UIComponentType::InspectorPanel>();
        //m_ImguiManager->RenderUIComponent<SPW::UIComponentType::Axis>();

        //----------------------------------------------------------------------------------------
    	// const auto& activeEntityPanel  = m_ImguiManager->GetEntityPanel();
        // const auto& activeInspector = m_ImguiManager->GetInspectorPanel();

    	//scene->forEachEntity<SPW::IDComponent>([this](const SPW::Entity& e)
     //   {
	    //    const auto component_name= e.component<SPW::NameComponent>()->getName();
	    //    const auto component_id  = e.component<SPW::IDComponent>()->getID().toString();
     //       m_ImguiManager->GetEntityPanel()->AddMenuItem(component_id, component_name, [&,e]()
     //       {
     //           m_ImguiManager->GetInspectorPanel()->SetSelectedGameObject(e);
     //       });
     //   });

    	m_ImguiManager->End();
        m_ImguiManager->EnableViewport();


        //-------------------------------------------------------------------------
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
    std::shared_ptr<SPW::ImGuiManager> m_ImguiManager;
    std::shared_ptr<SPW::SPWRenderSystem> renderSystem;
};

// main entrance
int main(int argc, char **argv) {
    // app test
    auto appProxy =
        SPW::Application::create<SPWTestApp>("SPWTestApp");
    return appProxy->app->run(argc, argv);
}
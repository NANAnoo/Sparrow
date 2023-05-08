//
// Created by dudu on 2023/3/31.
//
#include <iostream>
#include "Resource/Asset.hpp"
#include "Resource/AssetData/Mesh.h"
#include "ModelLoader/ModelLoader.h"
#include <fstream>

#include "IO/FileSystem.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"


#include <memory>
#include <sol/sol.hpp>

#include "EcsFramework/Entity/Entity.hpp"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include "SparrowCore.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Control/KeyCodes.h"

#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/ModelComponent.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"
#include "EcsFramework/System/RenderSystem/RenderSystem.h"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"

#include "Resource/AssetData/Model.h"

#include "Utils/UUID.hpp"

#include "EcsFramework/System/AudioSystem/AudioSystem.h"

#include "Platforms/OPENGL/OpenGLBackEnd.h"
#include "Platforms/OPENGL/OpenGLxGLFWContext.hpp"

#include "SimpleRender.h"
#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>

#include "ImGui/ImGuiManager.hpp"

#define SAVE
struct NoOpDeleter {
    template <typename T>
    void operator()(T*) const noexcept {}
};

// test usage
class Transformer : public SPW::WindowEventResponder
{
public:
    explicit Transformer(const std::shared_ptr<SPW::EventResponderI>& parent)
        : SPW::WindowEventResponder(parent) {
    }

    bool onWindowResize(int w, int h) override {
        std::cout << "window resize" << "(" << w << ", " << h << ")" << std::endl;
        bool should_update = false;
        if (w < 500) {
            w = 500;
            should_update = true;
        }
        if (h < 400) {
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
    bool onFrameResize(int w, int h) override {
        glViewport(0, 0, w, h);
        std::cout << "frame resize" << "(" << w << ", " << h << ")" << std::endl;
        // set projection
        return false;
    }
    const char* getName() final {
        return "Transformer";
    }
    std::weak_ptr<SPW::WindowI> window;
    std::weak_ptr<SPW::Scene> scene;

};

class SPWTestApp : public SPW::AppDelegateI
{
public:
    explicit SPWTestApp(std::shared_ptr<SPW::EventResponderI>& app, const char* name)
        : SPW::AppDelegateI(app), _name(name)
    {    }

    void onAppInit() final
    {
        std::shared_ptr<SPW::GlfwWindow> window = std::make_shared<SPW::GlfwWindow>();
        app->window = window;
        app->window->setSize(1280, 720);
        app->window->setTitle("OffineTool Test");

        transformer = std::make_shared<Transformer>(app->delegate.lock());
        transformer->window = window;

        // weak strong dance
        std::weak_ptr weak_window = window;
        window->onWindowCreated([weak_window, this](GLFWwindow* handle) {
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

            // add a camera entity
            auto camera = scene->createEntity("main camera");
            auto mainCameraTrans = camera->emplace<SPW::TransformComponent>();
            mainCameraTrans->position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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
            cam->whetherMainCam = true;
            //add a key component for testing, press R to rotate
            auto cameraKey = camera->emplace<SPW::KeyComponent>();
            auto cb = [](const SPW::Entity& e, SPW::KeyCode keycode) {
                auto mainCameraTrans = e.component<SPW::TransformComponent>();
                auto rotMat = glm::eulerAngleYXZ(glm::radians(mainCameraTrans->rotation.y),
                    glm::radians(mainCameraTrans->rotation.x),
                    glm::radians(mainCameraTrans->rotation.z));
                glm::vec4 front = { 0, 0, -1, 0 };
                front = rotMat * front;
                glm::vec3 forward = { front.x, 0, front.z };
                forward = glm::normalize(forward);
                glm::vec3 up = { 0, 1, 0 };
                glm::vec3 right = glm::normalize(glm::cross(forward, up));
                if (keycode == SPW::Key::W)
                    mainCameraTrans->position += 0.001f * forward;
                if (keycode == SPW::Key::S)
                    mainCameraTrans->position -= 0.001f * forward;
                if (keycode == SPW::Key::A)
                    mainCameraTrans->position -= 0.001f * right;
                if (keycode == SPW::Key::D)
                    mainCameraTrans->position += 0.001f * right;
                if (keycode == SPW::Key::Q)
                    mainCameraTrans->position -= 0.001f * up;
                if (keycode == SPW::Key::E)
                    mainCameraTrans->position += 0.001f * up;
            };
            auto mouse = camera->emplace<SPW::MouseComponent>();
            mouse->cursorMovementCallBack = [](const SPW::Entity& e, double x_pos, double y_pos, double x_pos_bias, double y_pos_bias) {
                auto transform = e.component<SPW::TransformComponent>();
                transform->rotation.x += y_pos_bias * 0.02;
                transform->rotation.y += x_pos_bias * 0.1;
            };
            cameraKey->onKeyHeldCallBack = cb;
            cameraKey->onKeyDownCallBack = cb;

            mouse->onMouseScrollCallBack = [](const SPW::Entity& e, double scroll_offset) {
                auto transform = e.component<SPW::TransformComponent>();
                transform->position.z += scroll_offset * 0.1;
            };


            // add light 1
            auto light = scene->createEntity("light");
            auto lightTrans = light->emplace<SPW::TransformComponent>();
            auto lightCom = light->emplace<SPW::DirectionalLightComponent>();
            lightCom->ambient = { 0.2, 0.2, 0.2 };
            lightCom->diffuse = { 1, 1, 0 };
            lightCom->specular = { 1, 1, 0 };
            lightTrans->rotation = { 0, 60, 0 };

            // add light 2
            auto light2 = scene->createEntity("light2");
            auto lightTrans2 = light2->emplace<SPW::TransformComponent>();
            auto lightCom2 = light2->emplace<SPW::DirectionalLightComponent>();
            lightCom2->ambient = { 0.2, 0.2, 0.2 };
            lightCom2->diffuse = { 0, 1, 1 };
            lightCom2->specular = { 0, 1, 1 };
            lightTrans2->rotation = { 0, -60, 0 };

            m_ImguiManager = std::make_shared<SPW::ImGuiManager>();
            m_ImguiManager->Init(handle);

            std::cout << "ImGui" << IMGUI_VERSION << std::endl;
#ifdef IMGUI_HAS_VIEWPORT
            std::cout << " +viewport";
#endif
#ifdef IMGUI_HAS_DOCK
            std::cout << " +docking" << std::endl;
#endif

            // init scene
            scene->initial();
            transformer->scene = scene;
            });
    }

    void beforeAppUpdate() final
    {
        scene->beforeUpdate();
    }
    void onAppUpdate(const SPW::TimeDuration& du) final {

        // physics, computation
        scene->onUpdate(du);
    }

    void afterAppUpdate() final {
        scene->afterUpdate();

        m_ImguiManager->Begin();
        m_ImguiManager->RenderUIComponent<SPW::UIComponentType::Dockspace>("Right Dock Space");
        m_ImguiManager->RenderUIComponent<SPW::UIComponentType::Dockspace>("Left Dock Space");
        m_ImguiManager->RenderUIComponent<SPW::UIComponentType::Dockspace>("Bottom Dock Space");
        m_ImguiManager->RenderUIComponent<SPW::UIComponentType::MenuBar>();
        m_ImguiManager->RenderUIComponent<SPW::UIComponentType::ObjectPanel>();
        m_ImguiManager->RenderUIComponent<SPW::UIComponentType::HierarchyPanel>();
        m_ImguiManager->RenderUIComponent<SPW::UIComponentType::InspectorPanel>();

        // const auto& activeEntityPanel  = m_ImguiManager->GetEntityPanel();
        // const auto& activeInspector = m_ImguiManager->GetInspectorPanel();
        scene->forEachEntity<SPW::IDComponent>([this](const SPW::Entity& e)
            {
                const auto component_name = e.component<SPW::NameComponent>()->getName();
                const auto component_id = e.component<SPW::IDComponent>()->getID().toString();
                m_ImguiManager->GetEntityPanel()->AddMenuItem(component_id, component_name, [&, e]()
                {
                    m_ImguiManager->GetInspectorPanel()->SetSelectedGameObject(e);
                });
            });

        m_ImguiManager->End();
        m_ImguiManager->EnableViewport();

    }
    void onUnConsumedEvents(std::vector<std::shared_ptr<SPW::EventI>>& events) final {
        // for (auto &e : events) {
        //     DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
        // }
    }
    void onAppStopped() final {
        sol::state state;
        state.open_libraries(sol::lib::base, sol::lib::package);
        std::string x = state["package"]["path"];
        state["package"]["path"] = x + ";./resources/scripts/lua/?.lua";
        try {
            if (state.script_file("./resources/scripts/lua/test.lua").valid()) {
                sol::protected_function main_function = state["main"];
                sol::protected_function_result result = main_function();
                if (!result.valid()) {
                    std::cout << "execution error" << std::endl;
                }
            }
        }
        catch (sol::error& e) {
            std::cout << e.what() << std::endl;
        }
        std::cout << "app stopped" << std::endl;
        scene->onStop();
    }

    void solveEvent(const std::shared_ptr<SPW::EventI>& e) final {
        e->dispatch<SPW::WindowCloseType, SPW::WindowEvent>(
            [this](SPW::WindowEvent* e) {
                // close application
                app->stop();
                return true;
            });
        SPW::EventResponderI::solveEvent(e);
    }

    const char* getName() final { return _name; }
    const char* _name;
    std::shared_ptr<Transformer> transformer;
    std::shared_ptr<SimpleRender> render;
    std::shared_ptr<SPW::Scene> scene;
    std::shared_ptr<SPW::RenderBackEndI> renderBackEnd;
    std::shared_ptr<SPW::ImGuiManager> m_ImguiManager;
};

// main entrance
int main(int argc, char** argv)
{
    // - Simulate a proces of a engine boost----------------------------------------------

// ConfigManager recevice the path of root, and construct the file pathes

// new a project file directory anywhere, assumse my destktop...
    const std::string root{ "C:/Users/dudu/Desktop/UserProject" };

    // FileSystem construct the different pathes
    const SPW::FileRoot projectRoots
    {
        root + "/engine",
        root + "/resource",
        root + "/asset"
    };


    // create the basic direcotories /engine and /resource /asset
    // TODO check the slash status
    SPW::FileSystem::CreateDirectory(projectRoots.engine);
    /*
     * copy engine files into /engine
     */

    SPW::FileSystem::CreateDirectory(root + "/resource");
    /*
     * copy project asset resource, including textures ... meshes
     */

    SPW::FileSystem::CreateDirectory(root + "/asset");
    // -----------------------------------------------

    // - Simulate a proces of loading some resources into a scene ------


#if defined SAVE
    // assume load two models into the scene
    std::unique_ptr<SPW::ModelData> model_0 = SPW::ModelLoader::LoadModel("C:/Users/dudu/Downloads/mosquito_in_amber/scene.gltf");
    std::unique_ptr<SPW::ModelData> model_1 = SPW::ModelLoader::LoadModel("C:/Dev/Sparrow Renderer/res/Models/mantis/model.fbx");

    // - Simulate a proces of save a scene into resources and asset

    // construct models 
    std::vector<SPW::Model> models = { model_0->model, model_1->model };

    // construct materials
    std::vector<SPW::Material> materials = model_0->materials;
    materials.insert(materials.end(), model_1->materials.begin(), model_1->materials.end());
    
    // construct textures
    std::unordered_map<std::string, std::string> textures = model_0->textures;
    textures.insert(model_1->textures.begin(), model_1->textures.end());


    std::ofstream modelfile(projectRoots.asset + "/scene.json");
    cereal::JSONOutputArchive modeldata_archive(modelfile);

    // Copy 
    SPW::FileSystem::CreateDirectory(projectRoots.resource + "/textures");

    for (auto& [k, v] : textures)
    {
        std::cout << k << ": " << v << std::endl;

        std::string destinationFilePath(projectRoots.resource + "/textures/" + SPW::FileSystem::ToFsPath(v).filename().string());
        SPW::FileSystem::CopyFile(v, destinationFilePath);

        // Update Texture Path
        v = destinationFilePath;
    }

    SPW::CameraComponent camera(SPW::PerspectiveType);
    SPW::TransformComponent transform{};
    SPW::PointLightComponent point_light{};
    SPW::DirectionalLightComponent directional_light{};
    modeldata_archive
    (
        cereal::make_nvp("models", models),
        cereal::make_nvp("materials", materials),
        cereal::make_nvp("textures", textures),
        cereal::make_nvp("camera", camera),
        cereal::make_nvp("transform", transform),
        cereal::make_nvp("point_light", point_light),
        cereal::make_nvp("directional_light", directional_light)
    );

    SPW::FileSystem::CreateDirectory(projectRoots.resource + "/mesh");
    {
        // serlierze mesh sperartely
        {
            std::ofstream mesh_bin(std::string(projectRoots.resource + "/mesh/" + model_0->model.meshURI + ".mesh"), std::ios::binary);
            cereal::BinaryOutputArchive archive(mesh_bin);
            archive(cereal::make_nvp(model_0->model.meshURI, model_0->meshes));
        }

        // serlierze mesh sperartely
        {
            std::ofstream mesh_bin(std::string(projectRoots.resource + "/mesh/" + model_1->model.meshURI + ".mesh"), std::ios::binary);
            cereal::BinaryOutputArchive archive(mesh_bin);
            archive(cereal::make_nvp(model_1->model.meshURI, model_1->meshes));
        }
    }
#elif defined LOAD

    // Load a scene from the disk
    std::vector<SPW::Model> models;
    std::vector<SPW::Material> materials;
    std::vector<SPW::Mesh> meshes;
    std::unordered_map<std::string, std::string>	textures;

    std::ifstream is_Scene(root + "/asset" + "/scene.json");
    cereal::JSONInputArchive ar(is_Scene);
    ar(
        cereal::make_nvp("models", models),
        cereal::make_nvp("materials", materials),
        cereal::make_nvp("textures", textures)
    );

    for (auto& model : models)
    {
        // load models from asset
        std::vector<SPW::Mesh> t_meshes;
        std::ifstream mesh_bin(std::string(projectRoots.resource + "/mesh/" + model.meshURI + ".mesh"), std::ios::binary);
        cereal::BinaryInputArchive ar(mesh_bin);
        ar(cereal::make_nvp(model.meshURI, t_meshes));

        meshes.insert(meshes.end(), t_meshes.begin(), t_meshes.end());
    }


    __debugbreak();
#endif

    // TODO DONE meshes json INPUT

    // TODO meshes binary OUTPUT

    // TODO DONE meshes binary INPUT
    // std::vector<SPW::Mesh> readin_meshes;
    // std::ifstream ismesh_bin("C:/Users/dudu/Desktop/model_meshes.bin");
    // cereal::JSONInputArchive is_mesh_bin(ismesh_bin);
    // is_mesh_bin(cereal::make_nvp("meshes", readin_meshes));



    // std::ofstream file("C:/Users/dudu/Desktop/model.json", std::ios::binary);
    // std::ofstream file("C:/Users/dudu/Downloads/mosquito_in_amber/scene.spw", std::ios::binary);



    // std::filesystem::path sourceFilePath("C:/Users/dudu/Downloads/mosquito_in_amber/textures/material_baseColor.jpeg");
    // std::filesystem::path destinationFilePath(projectRoots.resource + "/textures/" + sourceFilePath.filename().string());
    //
    // try 
    // {
    // 	std::filesystem::copy_file(sourceFilePath, destinationFilePath, std::filesystem::copy_options::overwrite_existing);
    // 	std::cout << "File copied successfully from " << sourceFilePath << " to " << destinationFilePath << std::endl;
    // }
    // catch (const std::filesystem::filesystem_error& e) {
    // 	std::cerr << "Error: " << e.what() << std::endl;
    // }

    // return 0;
    // __debugbreak();

    std::cout << "Hello, World!" << std::endl;

    // return 0;


	// app test
    auto appProxy =
        SPW::Application::create<SPWTestApp>("SPWTestApp");
    return appProxy->app->run(argc, argv);
}
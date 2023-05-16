#include <iostream>

#include "SparrowCore.h"
#include "Asset/ResourceManager/ResourceManager.h"
#include "IO/ConfigManager.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Event/Event.h"

#include "EcsFramework/Scene.hpp"
#include "Platforms/OPENGL/OpenGLxGLFWContext.hpp"

#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"

#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"
#include "Platforms/OPENGL/OpenGLBackEnd.h"


#include "EcsFramework/System/NewRenderSystem/SPWRenderSystem.h"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"
#include "EcsFramework/System/AudioSystem/AudioSystem.h"
#include "EcsFramework/System/PhysicSystem/PhysicSystem.h"
#include "EcsFramework/System/AnimationSystem/AnimationSystem.h"

#include "Render/shader.h"

#include "LuaBinding/SceneWrapper.hpp"
#include "LuaBinding/ComponentWrapper.hpp"
#include "LuaBinding/EntityWrapper.hpp"
#include "LuaBinding/RenderWrapper.hpp"

#include "LuaAppContext.hpp"


extern "C"{
#include <luasocket.h>
}

namespace SPW {
    class GameWrapper : public SPW::AppDelegateI {
    public:
        explicit GameWrapper(std::shared_ptr<SPW::EventResponderI> &app, std::string script_path = "")
    		: AppDelegateI(app), activePath(script_path)
    	{ }
        void onAppInit() final {
            auto window = std::make_shared<SPW::GlfwWindow>();
            app->window = window;
            std::weak_ptr<SPW::GlfwWindow> weak_window = window;
            window->onWindowCreated([this, weak_window](GLFWwindow *handle){
                if (weak_window.expired()) {
                    return;
                }
                // create graphics context
                weak_window.lock()->graphicsContext = std::make_shared<SPW::OpenGLxGLFWContext>(handle);
                // initial context
                weak_window.lock()->graphicsContext->Init();
                // create render back end
                renderBackEnd = std::make_shared<SPW::OpenGLBackEnd>();
                try {
                    context.onInit();
                } catch (sol::error &e) {
                    std::cout << e.what() << std::endl;
                }
            });
            
            // load lua script
            context.init(8818);
            // TODO : load entry file
            if (activePath.empty())
                return;
            context.loadEntryScript(activePath);
//            context.loadEntryScript("./resources/scripts/lua/TestGame.lua");

            window->setSize(context.app_width, context.app_height);
            window->setTitle(context.app_name);
            m_name = context.app_name;

            // create SPW root table
            auto cpp_table = context.getTable("SPW");
            // bind wrappers
            SPW::SceneWrapper::bindLuaTable(cpp_table);
            SPW::EntityWrapper::bindLuaTable(cpp_table);
            SPW::ComponentWrapper::bindLuaTable(cpp_table);
            SPW::ShaderDescWrapper::bindShaderDescWrapper(cpp_table);
            SPW::RenderNodeWrapper::bindLuaTable(cpp_table);
            SPW::RenderGraphWrapper::bindLuaTable(cpp_table);
            SPW::bindAttachmentPort(cpp_table);

            // bind shader handle
            cpp_table.new_usertype<SPW::ShaderHandle>("ShaderHandle",
                    sol::call_constructor, sol::constructors<SPW::ShaderHandle(const std::string&, const std::string&, const std::string&)>(),
                    sol::meta_function::to_string,[] (const SPW::ShaderHandle* shader) {return shader->name;},
                    "name", &SPW::ShaderHandle::name,
                    "vertex", &SPW::ShaderHandle::vertex_shader_path,    
                    "fragment", &SPW::ShaderHandle::frag_shader_path                
            );

            cpp_table["SceneWrapper"] = [this, weak_window](const std::string &path){
                int width = weak_window.lock()->frameWidth();
                int height = weak_window.lock()->frameHeight();
                auto scene = SPW::SceneWrapper(app, path);
                scene.m_scene->addSystem(std::make_shared<SPW::KeyControlSystem>(scene.m_scene));
                scene.m_scene->addSystem(std::make_shared<SPW::MouseControlSystem>(scene.m_scene));
                scene.m_scene->addSystem(std::make_shared<SPW::AnimationSystem>(scene.m_scene));
                scene.m_scene->addSystem(std::make_shared<SPW::AudioSystem>(scene.m_scene));
                scene.m_scene->addSystem(std::make_shared<SPW::PhysicSystem>(scene.m_scene));
                auto renderSystem = std::make_shared<SPW::SPWRenderSystem>(scene.m_scene, renderBackEnd, width, height);
                scene.setUpDefaultRenderGraph(renderSystem, renderBackEnd);
                scene.m_scene->addSystem(renderSystem);
                return scene;
            };
            cpp_table["LoadAsset"] = [this, weak_window](const std::string &name) {
                auto data = SPW::AssetManager::LoadAsset(SPW::Config::k_WorkingProjectAssets + name);
			    SPW::ResourceManager::getInstance()->m_AssetDataMap.emplace(data.assetName, data);
            };
        }
        void beforeAppUpdate() final{
            try {
                context.beforeUpdate();
            } catch (sol::error &e) {
                std::cout << e.what() << std::endl;
            }
        }
        void onAppUpdate(const SPW::TimeDuration &du) final{
            try {
                context.onUpdate(du.toSecond());
            } catch (sol::error &e) {
                std::cout << e.what() << std::endl;
            }
        }

        void afterAppUpdate() final{
            try {
                context.afterUpdate();
            } catch (sol::error &e) {
                std::cout << e.what() << std::endl;
            }
        }
        void onUnConsumedEvents(std::vector<std::shared_ptr<SPW::EventI>> &events) final{
            for (auto &e : events) {
                DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
            }
        }
        void onAppStopped() final{
            try {
                context.onStop();
            } catch (sol::error &e) {
                std::cout << e.what() << std::endl;
            }
        }

        void solveEvent(const std::shared_ptr<SPW::EventI> &e) final {
            e->dispatch<SPW::WindowCloseType, SPW::WindowEvent>(
                    [this](SPW::WindowEvent *e){
                        // close application
                        app->stop();
                        return true;
                    });
            e->dispatch<SPW::WindowResizeType, SPW::WindowEvent>(
                    [this](SPW::WindowEvent *e){
                        // resize window
                        try {
                            context.onWindowResize(e->width, e->height);
                        } catch (sol::error &e) {
                            std::cout << e.what() << std::endl;
                        }
                        return false;
                    });
            SPW::EventResponderI::solveEvent(e);
        }

        const char *getName() final {return m_name.c_str();}
        std::string m_name;
        LuaAppContext context;
        std::shared_ptr<RenderBackEndI> renderBackEnd;
        std::string activePath;
    };
}

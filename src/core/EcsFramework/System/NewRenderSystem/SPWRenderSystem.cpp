#include "SPWRenderSystem.h"
#include "EcsFramework/Component/ComponentI.h"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/MeshComponent.hpp"

#include "Render/Light.h"
#include "Utils/Debounce.hpp"
#include "Asset/BasicMeshStorage.hpp"

#include "Render/RenderGraphManager.h"

#include <glm/glm/gtx/euler_angles.hpp>
#include "IO/FileSystem.h"

namespace SPW {
    void SPWRenderSystem::initial() {
        renderBackEnd->Init();
        
        renderBackEnd->loadShaderLib("./resources/shaders/baselib/");

        screenBuffer = renderBackEnd->createFrameBuffer();

        screenBuffer->genFrameBuffer();
        screenBuffer->bind();
        screenBuffer->AttachColorRenderBuffer(width, height, 0);
        screenBuffer->AttachDepthRenderBuffer(width, height);

        screenTexture = renderBackEnd->createAttachmentTexture();
        screenTexture->genTexture(width, height, RGB32);
        screenTexture->attach(0);

        screenDepthTexture = renderBackEnd->createAttachmentTexture();
        screenDepthTexture->genTexture(width, height, Depth);
        screenDepthTexture->attach(0);
        screenDepthTexture->unbind();

        screenBuffer->CheckFramebufferStatus();
        screenBuffer->unbind();

        renderBackEnd->SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

        RenderGraphManager::getInstance()->initializeGraph(width, height);
        skyBoxGraph->init(width, height);
        postProcessGraph->init(width, height);
        uiGraph->init(width, height);
    }

    void SPWRenderSystem::beforeUpdate() 
    {
        locatedScene.lock()->forEach(
        [this](MeshComponent *meshComponent){
            if (!meshComponent->ready)  
			{
                if ( ResourceManager::getInstance()->m_AssetDataMap.contains(meshComponent->assetName)) {
                    auto& meshes = ResourceManager::getInstance()->m_AssetDataMap[meshComponent->assetName].meshes;
                    for (auto& mesh : meshes)
                        mesh.setupMesh(renderBackEnd);
                }
                BasicMeshStorage<UIMesh>::getInstance()
                ->forEachMesh(
                        meshComponent->assetID,
                        [this](auto mesh,auto m, auto t)
                        {
                            mesh->setupMesh(renderBackEnd);
                });
                meshComponent->ready = true;
            }
            
            if (meshComponent->beforeDraw) {
                RenderCommandsQueue<RenderBackEndI> queue;
				meshComponent->beforeDraw(queue);
                queue.executeWithAPI(renderBackEnd);
            }
			meshComponent->beforeDraw = nullptr;
        }, MeshComponent);
    }

    void SPWRenderSystem::afterUpdate()
    {
        //TICKTOCK;
        // clear screen and clear screen buffer
        screenBuffer->bind();
        renderBackEnd->Clear();
        screenBuffer->unbind();
        renderBackEnd->Clear();
        renderBackEnd->DepthTest(true);
        // find all cameras
        ComponentGroup<IDComponent, CameraComponent, TransformComponent> cameraGroup;
        std::vector<Entity> cameras;
        Entity uiCamera = Entity::nullEntity();

        locatedScene.lock()->forEachEntityInGroup(cameraGroup, 
            [&cameras, &uiCamera](const Entity &en){
            if (en.component<CameraComponent>()->getType() != CameraType::UIOrthoType) {
                cameras.push_back(en);
            } else {
                uiCamera = en;
            }
        });

        if (!uiCamera.isNull()) {
            // make sure ui camera is the last one
            cameras.push_back(uiCamera);
        }

        // find all meshes that belong to each camera
        std::vector<std::vector<Entity>> models_by_camera(cameras.size());
        ComponentGroup<IDComponent, MeshComponent, TransformComponent> meshGroup;
        // camera loop
        for (unsigned int i = 0; i < cameras.size(); ++i) {
            UUID camID = cameras[i].getUUID();
            locatedScene.lock()->forEachEntityInGroup(meshGroup,
                [&camID, &models_by_camera, i](const Entity& en)
                {
                    auto mesh = en.component<MeshComponent>();
                	if(mesh->bindCamera == camID)
                    {
                        models_by_camera[i].push_back(en);
					}
                });
        }

        // build up render input
        RenderInput input{};
        input.backend = renderBackEnd;
        input.depth_texture = screenDepthTexture;
        input.screen_texture = screenTexture;
        input.screen_buffer = screenBuffer;
        input.shader_map = shader_storage;
        input.screen_width = width;
        input.screen_height = height;
        findAllLights(input.dLights, input.pLights);
        // draw each camera
        for (unsigned int i = 0; i < cameras.size(); ++ i) {
            // draw render passes
            auto cam = cameras[i];
            auto meshes = models_by_camera[i];
            if (meshes.empty())
                continue;
            // calculate view and projection matrix
            auto cameraCom = cam.component<CameraComponent>();
            auto transformCom = cam.component<TransformComponent>();
            glm::mat4x4 V, P;
            glm::vec3 camPos = transformCom->position;
            glm::mat4x4 cameraTransform = glm::translate(glm::mat4(1.f), transformCom->position);
            cameraTransform = cameraTransform * glm::eulerAngleYXZ(glm::radians(transformCom->rotation.y),
                                glm::radians(transformCom->rotation.x),
                                glm::radians(transformCom->rotation.z));

            glm::vec4 eye(0, 0, 0, 1), look_at(0, 0, -1, 1), up(0, 1, 0, 0);
            glm::vec3 cam_center = cameraTransform *look_at;
            V = glm::lookAt(glm::vec3(cameraTransform * eye),
                            cam_center,
                            glm::vec3(cameraTransform * up));
            if (cameraCom->getType() == SPW::PerspectiveType) {
                P = glm::perspective(glm::radians(cameraCom->fov),
                                    cameraCom->aspect,
                                    cameraCom->near,
                                    cameraCom->far);

            } else {
                P = glm::ortho(cameraCom->left, cameraCom->right,
                            cameraCom->bottom, cameraCom->top, cameraCom->near, cameraCom->far);
            }
            input.camera_pos = camPos;
            input.camera_dir = cam_center - camPos;
            input.view = V;
            input.projection = P;
            // map meshes by shader desc
            std::unordered_map<unsigned int, std::unordered_map<unsigned int, std::vector<Entity>>> model_by_pass;
            for (const auto& mesh_entity : meshes) {
                auto mesh = mesh_entity.component<MeshComponent>();
            	auto &model_by_shader = model_by_pass[mesh->bindRenderGraph];
                for (auto &[id, uuid] : mesh->modelSubPassPrograms) {
                    model_by_shader[id].push_back(mesh_entity);
                }
            }

            RenderGraphManager::getInstance()->forEachGraph(
                    [&model_by_pass, &input](const std::shared_ptr<RenderGraph> &graph){
                auto graph_id = graph->graph_id;
                if (model_by_pass.find(graph_id) != model_by_pass.end()) {
                    input.sourceType = SPW::MeshSourceType::MeshFromAsset;
                    input.render_models = model_by_pass.at(graph_id);
                    graph->render(input);
                }
            });

            if (model_by_pass.find(skyBoxGraph->graph_id) != model_by_pass.end()) {
                input.sourceType = SPW::MeshSourceType::MeshFromAsset;
                input.render_models = model_by_pass.at(skyBoxGraph->graph_id);
                skyBoxGraph->render(input);
            }

            if (cameraCom->getType() == SPW::UIOrthoType && model_by_pass.find(uiGraph->graph_id) != model_by_pass.end()) {
                // draw ui
                input.sourceType = SPW::MeshSourceType::MeshFromUIStorage;
                input.render_models = model_by_pass.at(uiGraph->graph_id);
                uiGraph->render(input);
            }
        }

        postProcessGraph->render(input);

        locatedScene.lock()->forEach(
        [](MeshComponent *mesh){
            mesh->onDraw = nullptr;
        }, MeshComponent);
    }

    void SPWRenderSystem::onStop()
    {

    }

    bool SPWRenderSystem::onFrameResize(int w, int h)
    {
        width = w;
        height = h;
        static Debouncer updateFrame(*MainThreadExecutor::getInstance(), [this](){
            screenBuffer = renderBackEnd->createFrameBuffer();
            screenBuffer->genFrameBuffer();
            screenBuffer->bind();
            screenBuffer->AttachColorRenderBuffer(width, height, 0);
            screenBuffer->AttachDepthRenderBuffer(width, height);

            screenTexture = renderBackEnd->createAttachmentTexture();
            screenTexture->genTexture(width, height, RGB32);
            screenTexture->attach(0);

            screenDepthTexture = renderBackEnd->createAttachmentTexture();
            screenDepthTexture->genTexture(width, height, Depth);
            screenDepthTexture->attach(0);
            screenDepthTexture->unbind();

            screenBuffer->CheckFramebufferStatus();
            screenBuffer->unbind();

            RenderGraphManager::getInstance()->onFrameChanged(width, height);
            uiGraph->onFrameChanged(width, height);
            postProcessGraph->onFrameChanged(width, height);
            skyBoxGraph->onFrameChanged(width, height);

        }, 100);

        updateFrame();
        
        return false;
    }
    void SPWRenderSystem::findAllLights(std::vector<DLight> &dLights, std::vector<PLight> &pLights)
    {
        // get directional lights
        ComponentGroup<IDComponent,
            DirectionalLightComponent,
            TransformComponent> lightGroup;
        locatedScene.lock()->forEachEntityInGroup(lightGroup, [&lightGroup, &dLights](const Entity &en){
            auto [id, light, trans] = en.combinedInGroup(lightGroup);
            glm::vec4 dir = {0, 0, -1, 0};
            auto rotMat = glm::eulerAngleXYZ(glm::radians(trans->rotation.x),
                        glm::radians(trans->rotation.y),
                        glm::radians(trans->rotation.z));
            DLight dl = {};
            dl.direction = dir * rotMat;
            dl.ambient = light->ambient;
            dl.diffuse = light->diffuse;
            dl.specular = light->specular;
            dLights.push_back(dl);
        });

        // get point lights
        ComponentGroup<IDComponent,
            PointLightComponent,
            TransformComponent> pointLightGroup;
        locatedScene.lock()->forEachEntityInGroup(pointLightGroup, [&pointLightGroup, &pLights](const Entity &en) {
            auto [id, light, trans] = en.combinedInGroup(pointLightGroup);
            PLight pl = {};
            pl.position = trans->position;
            pl.ambient = light->ambient;
            pl.diffuse = light->diffuse;
            pl.specular = light->specular;
            pl.constant = light->constant;
            pl.linear = light->linear;
            pl.quadratic = light->quadratic;
            pLights.push_back(pl);
        });
    }
}

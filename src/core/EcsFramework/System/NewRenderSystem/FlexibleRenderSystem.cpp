#include "FlexibleRenderSystem.h"
#include "EcsFramework/Component/ComponentI.h"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/ModelComponent.h"
#include "glm/fwd.hpp"
#include "Render/Light.h"

#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>
#include <string>

namespace SPW {

    void FlexibleRenderSystem::initial() {
        screenBuffer = renderBackEnd->createFrameBuffer();
        screenBuffer->bind();
        screenBuffer->AttachColorRenderBuffer(width, height, 0);
        screenBuffer->AttachDepthRenderBuffer(width, height);
        screenBuffer->AttachColorTexture(width, height, 0);
        screenBuffer->CheckFramebufferStatus();
        screenBuffer->unbind();

        // init sub passes
        for (auto &subPass : ModelPasses) {
            subPass.init(renderBackEnd, width, height);
        }

        // init image passes
        for (auto &subPass : ImagePasses) {
            subPass.init(renderBackEnd, width, height);
        }

    }

    void FlexibleRenderSystem::beforeUpdate() {
        locatedScene.lock()->forEach(
            [this](MeshComponent *mesh){
                if (!mesh->ready) {
                    mesh->model->setUpModel(renderBackEnd);
                    mesh->ready = true;
                }
        }, MeshComponent);
    }

    void FlexibleRenderSystem::onUpdate(TimeDuration dt) {

    }

    void FlexibleRenderSystem::afterUpdate() {
        findAllLights();
        screenBuffer->bind();
        renderBackEnd->SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
        renderBackEnd->Clear();
        screenBuffer->unbind();

        // find all cameras
        ComponentGroup<IDComponent, CameraComponent, TransformComponent> cameraGroup;
        std::vector<Entity> cameras;
        std::vector<Entity> uiMeshes;
        Entity uiCamera = Entity::nullEntity();
        locatedScene.lock()->forEachEntityInGroup(cameraGroup, 
            [&cameraGroup, &cameras, &uiCamera](const Entity &en){
            if (en.component<CameraComponent>()->getType() != CameraType::UIOrthoType) {
                cameras.push_back(en);
            } else {
                uiCamera = en;
            }
        });

        // find all meshes that belong to each camera
        std::vector<std::vector<Entity>> models_by_camera(cameras.size());
        ComponentGroup<IDComponent, MeshComponent, TransformComponent> meshGroup;
        // camera loop
        for (unsigned int i = 0; i < cameras.size(); ++ i) {
            UUID camID = cameras[i].getUUID();
            locatedScene.lock()->forEachEntityInGroup(meshGroup, 
                [&meshGroup, &camID, &models_by_camera, i](const Entity &en){
                if (en.component<MeshComponent>()->bindCamera == camID) {
                    models_by_camera[i].push_back(en);
                }
            });
        }
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

            for (auto &pass : RenderPasses) {
                std::vector<Entity> meshes_in_pass;
                for (auto &mesh : meshes) {
                    if (mesh.component<MeshComponent>()->bindRenderPass == pass.id) {
                        meshes_in_pass.push_back(mesh);
                    }
                }
                if (meshes_in_pass.empty()) 
                    continue;
                
                renderModelSubPasses(V, P, meshes_in_pass, pass.model_subpass);
                renderImageSubPasses(pass.model_subpass.back(), pass.image_subpass);
            }
        }
        // render ui
    }

    void FlexibleRenderSystem::onStop() {

    }

    bool FlexibleRenderSystem::onFrameResize(int w, int h) {
        // update frame buffer here
        width = w;
        height = h;
        screenBuffer->deleteFrameBuffer();
        screenBuffer->genFrameBuffer();
        screenBuffer->bind();
        screenBuffer->AttachColorRenderBuffer(width, height, 0);
        screenBuffer->AttachColorTexture(w,h,0);
        screenBuffer->AttachDepthRenderBuffer(w,h);
        screenBuffer->CheckFramebufferStatus();
        screenBuffer->unbind();

        // re init sub passes
        for (auto &subPass : ModelPasses) {
            if (subPass.hasInternalFramebuffer() && (subPass.config.frameWidth == 0 || subPass.config.frameHeight == 0))
                subPass.init(renderBackEnd, width, height);
        }
        
        // re init image passes
        for (auto &subPass : ImagePasses) {
            if (subPass.hasInternalFramebuffer() && (subPass.config.frameWidth == 0 || subPass.config.frameHeight == 0))
                subPass.init(renderBackEnd, width, height);
        }
        return false;
    }

    void FlexibleRenderSystem::renderModelSubPasses(
            const glm::mat4x4 &V, const glm::mat4x4 &P,
            const std::vector<Entity> &models, 
            const std::vector<unsigned int> &subPassIDs) {
        // render models
        for (unsigned int id : subPassIDs) {
            SPWModelSubPass &subPass = ModelPasses[id];
            // mesh order by shader
            ShaderModelMap shaderModelMap;
            for (auto &mesh : models) {
                auto meshCom = mesh.component<MeshComponent>();
                if (meshCom->modelSubPassProgram.find(id) != meshCom->modelSubPassProgram.end()) {
                    shaderModelMap[meshCom->modelSubPassProgram[id]].push_back(mesh);
                }
            }
            // draw per shader
            if (shaderModelMap.empty())
                continue;
            // bind corresponding framebuffer
            if (subPass.hasInternalFramebuffer()) {
                // bind internal framebuffer
                if (subPass.config.repeatType == NoRepeat) {
                    subPass.attchmentFrameBuffers[0]->bind();
                    renderBackEnd->SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
                    renderBackEnd->Clear();
                    AttachmentPack &attachment = subPass.attchmentTextures[0];
                    unsigned int attachIndex = 0;
                    if (attachment.textureCube) {
                        for (unsigned int face = 0; face < 6; face ++) {
                            for (unsigned int attachIndex = 0; attachIndex < subPass.attchmentTextures.size(); attachIndex ++) {
                                assert(subPass.attchmentTextures[attachIndex].textureCube == nullptr);
                                subPass.attchmentTextures[attachIndex].textureCube->attach(attachIndex, face);
                            }
                            // check framebuffer status
                            subPass.attchmentFrameBuffers[0]->CheckFramebufferStatus();
                            // draw per shader
                            renderModelSubPass(V, P, shaderModelMap, id);

                        }
                    } else {
                        // draw per shader
                        renderModelSubPass(V, P, shaderModelMap, id);
                    }
                } else {
                    int repeat_count = subPass.config.repeatCount;
                    if ((subPass.config.repeatType & RepeatForPLights) > 0) {
                        int max_count = std::min(repeat_count, int(pLights.size()));
                        for (unsigned int i = 0; i < max_count; i ++) {
                            subPass.attchmentFrameBuffers[i]->bind();
                            renderBackEnd->SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
                            renderBackEnd->Clear();
                            AttachmentPack &attachment = subPass.attchmentTextures[0];
                            unsigned int attachIndex = 0;
                            if (attachment.textureCubeArray) {
                                for (unsigned int face = 0; face < 6; face ++) {
                                    for (unsigned int attachIndex = 0; attachIndex < subPass.attchmentTextures.size(); attachIndex ++) {
                                        assert(subPass.attchmentTextures[attachIndex].textureCubeArray == nullptr);
                                        subPass.attchmentTextures[attachIndex].textureCubeArray->attach(attachIndex, i, face);
                                    }
                                    // check framebuffer status
                                    subPass.attchmentFrameBuffers[i]->CheckFramebufferStatus();
                                    // draw per shader with pLights[i]
                                    renderModelSubPassOnPlight(V, P, shaderModelMap, id, i);
                                }
                                
                            } else {
                                // draw per shader pLights[i]
                                renderModelSubPassOnPlight(V, P, shaderModelMap, id, i);
                            }
                            subPass.attchmentFrameBuffers[i]->unbind();
                        }
                    }
                    if ((subPass.config.repeatType & RepeatForDLights) > 0) {
                        int max_count = std::min(repeat_count, int(dLights.size()));
                        for (unsigned int i = 0; i < max_count; i ++) {
                            subPass.attchmentFrameBuffers[i]->bind();
                            renderBackEnd->SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
                            renderBackEnd->Clear();
                            AttachmentPack &attachment = subPass.attchmentTextures[0];
                            unsigned int attachIndex = 0;
                            if (attachment.textureCubeArray) {
                                for (unsigned int face = 0; face < 6; face ++) {
                                    for (unsigned int attachIndex = 0; attachIndex < subPass.attchmentTextures.size(); attachIndex ++) {
                                        assert(subPass.attchmentTextures[attachIndex].textureCubeArray == nullptr);
                                        subPass.attchmentTextures[attachIndex].textureCubeArray->attach(attachIndex, i, face);
                                    }
                                    // check framebuffer status
                                    subPass.attchmentFrameBuffers[i]->CheckFramebufferStatus();
                                    // draw per shader with dLights[i]
                                    renderModelSubPassOnDlight(V, P, shaderModelMap, id, i);
                                }
                                
                            } else {
                                // draw per shader
                                renderModelSubPassOnDlight(V, P, shaderModelMap, id, i);
                            }
                            subPass.attchmentFrameBuffers[i]->unbind();
                        }
                    }
                }
            } else {
                // draw on screen buffer
                if (subPass.config.outputType == ScreenType) {
                    screenBuffer->bind();
                    // draw per shader
                    renderModelSubPass(V, P, shaderModelMap, id);
                    screenBuffer->unbind();
                } else if (subPass.config.outputType == PresentType) {
                    screenBuffer->unbind();
                    // present directly
                    renderModelSubPass(V, P, shaderModelMap, id);
                }
                // should be the last subpass
                return;
            }
        }
    }

    void FlexibleRenderSystem::renderModelSubPass(
        const glm::mat4x4 &V, const glm::mat4x4 &P, 
        const ShaderModelMap &models, unsigned int subPassID)
    {

    }
    void FlexibleRenderSystem::renderModelSubPassOnDlight(
        const glm::mat4x4 &V, const glm::mat4x4 &P, 
        const ShaderModelMap &models, unsigned int subPassID, unsigned int dlightID)
    {

    }
    void FlexibleRenderSystem::renderModelSubPassOnPlight(
        const glm::mat4x4 &V, const glm::mat4x4 &P, ShaderModelMap &models, 
        unsigned int subPassID, unsigned int plightID)
    {

    }

    void FlexibleRenderSystem::findAllLights()
    {
        dLights.clear();
        pLights.clear();
        // get directional lights
        ComponentGroup<IDComponent,
            DirectionalLightComponent,
            TransformComponent> lightGroup;
        locatedScene.lock()->forEachEntityInGroup(lightGroup, [&lightGroup, this](const Entity &en){
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
        locatedScene.lock()->forEachEntityInGroup(pointLightGroup, [&pointLightGroup, this](const Entity &en) {
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

#include "FlexibleRenderSystem.h"
#include "EcsFramework/Component/ComponentI.h"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/MeshComponent.hpp"
#include "glm/fwd.hpp"
#include "Render/Light.h"

#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>
#include <string>

namespace SPW {
    static const glm::mat4 getCubeViewOnFace(const glm::vec3 &pos, int face) {
        // calculate view matrix for face
        glm::mat4 view;
        switch (face) {
            case 0:
                view = glm::lookAt(pos, pos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0));
                break;
            case 1:
                view = glm::lookAt(pos, pos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
                break;
            case 2:
                view = glm::lookAt(pos, pos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
                break;
            case 3:
                view = glm::lookAt(pos, pos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
                break;
            case 4:
                view = glm::lookAt(pos, pos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));
                break;
            case 5:
                view = glm::lookAt(pos, pos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));
                break;
        }
        return view;
    }

    static const glm::mat4 getCubeProjection() {
        return glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
    }

    void FlexibleRenderSystem::initial() {
        renderBackEnd->Init();
        renderBackEnd->loadShaderLib("./resources/shaders/baselib");

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

        // // init sub passes
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
        // clear screen and clear screen buffer
        screenBuffer->bind();
        renderBackEnd->DepthTest(true);
        renderBackEnd->SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
        renderBackEnd->Clear();
        screenBuffer->unbind();
        renderBackEnd->SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
        renderBackEnd->Clear();
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
            glm::vec4 camPos = glm::vec4(transformCom->position, 1.f);
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
                
                renderModelSubPasses(camPos, cam_center, V, P, meshes_in_pass, pass.model_subpass);
                unsigned int finalPass = pass.model_subpass.size() > 0 ? pass.model_subpass.back() : 0;
                renderImageSubPasses(camPos, cam_center, finalPass, pass.image_subpass);
            }
        }

        // post process
        renderImageSubPasses(glm::vec4(0, 0, 0, 1), glm::vec3(0, 0, -1), 0, postProcessPass.image_subpass);
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
        screenBuffer->AttachDepthRenderBuffer(width, height);

        screenTexture->deleteTexture();
        screenTexture->genTexture(width, height, RGB32);
        screenTexture->attach(0);

        screenDepthTexture->deleteTexture();
        screenDepthTexture->genTexture(width, height, Depth);
        screenDepthTexture->attach(0);

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
        return false;
    }

    void FlexibleRenderSystem::renderModelSubPasses(
            const glm::vec4 &camPos, const glm::vec3 &cam_center,
            const glm::mat4x4 &view, const glm::mat4x4 &proj,
            const std::vector<Entity> &models, 
            const std::vector<unsigned int> &subPassIDs) 
    {
        // render models
        unsigned int prevID = 0;
        for (unsigned int id : subPassIDs) {
            SPWModelSubPass &subPass = ModelPasses[id - 1];
            // mesh order by shader
            ShaderModelMap shaderModelMap;
            for (auto &mesh : models) {
                auto meshCom = mesh.component<MeshComponent>();
                if (meshCom->modelSubPassProgram.find(id) != meshCom->modelSubPassProgram.end()) {
                    shaderModelMap[meshCom->modelSubPassProgram[id]].push_back(mesh);
                }
            }
            // TODO : check should return if shaderModelMap is empty
            // bind corresponding framebuffer
            if (subPass.hasInternalFramebuffer()) {
                // bind internal framebuffer
                if (subPass.config.repeatType == NoRepeat) {
                    subPass.attchmentFrameBuffers[0]->bind();
                    renderBackEnd->SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
                    renderBackEnd->Clear();
                    if (subPass.config.outputType == CubeMapType) {
                        // draw 6 faces
                        for (unsigned int face = 0; face < 6; face ++) {
                            // attach MRT texture
                            unsigned int attachIndex = 0;
                            for (unsigned int i = 0; i < subPass.attchmentTextures.size(); i ++) {
                                auto &texture = subPass.attchmentTextures[i];
                                auto format = subPass.config.attachments[i];
                                assert(texture.textureCube == nullptr);
                                if (format == Depth) {
                                    texture.textureCube->attach(0, face);
                                } else {
                                    texture.textureCube->attach(attachIndex ++, face);
                                }
                            }
                            // check framebuffer status
                            subPass.attchmentFrameBuffers[0]->CheckFramebufferStatus();
                            // draw per shader 
                            // TODO: what is camera here?
                            renderModelSubPass(glm::vec4(0.f), glm::vec3(0.f), 
                                getCubeViewOnFace(glm::vec3(0.f), face), getCubeProjection(), 
                                shaderModelMap, id, prevID, NoRepeat, 0);
                        }
                    } else if (subPass.config.outputType == ColorType){
                        renderModelSubPass(camPos, cam_center, view, proj, shaderModelMap, id, prevID, NoRepeat, 0);
                    } else {
                        assert(false);
                    }
                    subPass.attchmentFrameBuffers[0]->unbind();
                } else {
                    int repeat_count = subPass.config.repeatCount;
                    for (auto type : {RepeatForPLights, RepeatForDLights}) {
                        if ((subPass.config.repeatType & type) == 0) 
                            continue;
                        int max_count = repeat_count;
                        if (RepeatForPLights == type) {
                            max_count = std::min(max_count, int(pLights.size()));
                        } else if (RepeatForDLights == type) {
                            max_count = std::min(max_count, int(dLights.size()));
                        }
                        for (unsigned int i = 0; i < max_count; i ++) {
                            subPass.attchmentFrameBuffers[i]->bind();
                            renderBackEnd->SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
                            if (subPass.config.outputType == CubeMapType) {
                                for (unsigned int face = 0; face < 6; face ++) {
                                    // attach MRT texture
                                    unsigned int attachIndex = 0;
                                    for (unsigned int idx = 0; idx < subPass.attchmentTextures.size(); idx ++) {
                                        auto &texture = subPass.attchmentTextures[idx];
                                        auto format = subPass.config.attachments[idx];
                                        assert(texture.textureCubeArray != nullptr);
                                        if (format == Depth) {
                                            texture.textureCubeArray->attach(0, i, face);
                                        } else {
                                            texture.textureCubeArray->attach(attachIndex ++, i, face);
                                        }
                                    }
                                    // check framebuffer status
                                    subPass.attchmentFrameBuffers[i]->CheckFramebufferStatus();
                                    // draw per shader with pLights[i]
                                    glm::vec4 position = glm::vec4(0.f);
                                    if (RepeatForPLights == type) {
                                        position = glm::vec4(pLights[i].position, 1.f);
                                    }
                                    renderBackEnd->Clear();
                                    renderModelSubPass(
                                        position, glm::vec3(0.f), 
                                        getCubeViewOnFace(position, face), getCubeProjection(), 
                                        shaderModelMap, id, prevID, type, i);
                                }
                            } else if (subPass.config.outputType == ColorType) {
                                renderBackEnd->Clear();
                                // draw per shader dLights[i]
                                renderModelSubPass(camPos, cam_center, view, proj, shaderModelMap, id, prevID, type, i);
                            } else {
                                assert(false);
                            }
                            subPass.attchmentFrameBuffers[i]->unbind();
                        }
                    }
                }
            } else {
                if ((subPass.config.screenOutputType & ScreenAllType) > 0) {
                    // draw on screen buffer
                    screenBuffer->bind();
                    // draw per shader
                    renderModelSubPass(camPos, cam_center, view, proj, shaderModelMap, id, prevID, NoRepeat, 0);
                    screenBuffer->unbind();
                } else if (subPass.config.screenOutputType == PresentType) {
                    screenBuffer->unbind();
                    // present directly
                    renderModelSubPass(camPos, cam_center, view, proj, shaderModelMap, id, prevID, NoRepeat, 0);
                }
                // should be the last subpass
                return;
            }
            prevID = id;
        }
    }

    void FlexibleRenderSystem::renderModelSubPass(
        const glm::vec4 &camPos, const glm::vec3 &cam_center,
        const glm::mat4x4 &view, const glm::mat4x4 &proj, 
        const ShaderModelMap &models, unsigned int subPassID, unsigned int dependID, 
        const RepeatType repeatType, unsigned int repeatID)
    {
        assert(ModelPasses.size() >= subPassID);
        auto &subpass = ModelPasses[subPassID-1];
        subpass.setViewPort(renderBackEnd, width, height);
        
        for (auto &[shaderhandle, models] : models) {
            auto shader = renderBackEnd->getShader(shaderhandle);
            shader->Bind();
            subpass.bindContext(shader, width, height, camPos);
            if (repeatType == RepeatForDLights) {
                subpass.bindDlight(shader, cam_center, dLights[repeatID]);
            } else if (repeatType == RepeatForPLights) {
                subpass.bindPlight(shader, pLights[repeatID]);
            } else {
                subpass.bindPlights(shader, pLights);
                subpass.bindDlights(shader, cam_center, dLights);
            }
            unsigned int slot = 0;
            if (dependID > 0) {
                assert(ModelPasses.size() >= dependID);
                slot = subpass.bindPreviousAttachments(shader, ModelPasses[dependID - 1]);
            }
            // calculate model matrix
            for (auto &model : models) {
                auto modelCom = model.component<MeshComponent>();
                auto transformCom = model.component<TransformComponent>();

                glm::mat4 MM = glm::mat4(1.0f);
                MM = glm::translate(MM,transformCom->position);
                MM = MM * glm::eulerAngleYXZ(glm::radians(transformCom->rotation.y),
                                            glm::radians(transformCom->rotation.x),
                                            glm::radians(transformCom->rotation.z));
                MM = glm::scale(MM, transformCom->scale);
                subpass.bindMVP(shader, MM, view, proj);

                // draw submeshes
                for (auto &submesh : modelCom->model->GetMeshes()) {
                    subpass.bindMaterial(shader, renderBackEnd, submesh->GetMaterial(), slot);
                    submesh->PureDraw(renderBackEnd);
                }
            }
        }
        renderBackEnd->DepthFunc(DepthComp::LESS);
    }

    void FlexibleRenderSystem::renderImageSubPasses(
        const glm::vec4 &camPos, const glm::vec3 &cam_center, 
        unsigned int dependModelPass, const std::vector<unsigned int> &subPassIDs)
    {
        if (subPassIDs.empty()) return;
        unsigned int fisrtID = subPassIDs[0];
        unsigned int prevID = fisrtID;
        renderBackEnd->DepthTest(false);
        for (auto id : subPassIDs) {
            auto &subpass = ImagePasses[id - 1];
            if (subpass.hasInternalFramebuffer()) {
                subpass.internalFrameBuffer->bind();
            } else if (subpass.config.screenOutputType == PresentType) {
                screenBuffer->unbind();
            } else {
                screenBuffer->bind();
            }
            subpass.setViewPort(renderBackEnd, width, height);
            auto shader = renderBackEnd->getShader(subpass.config.shader);
            shader->Bind();
            subpass.bindContext(shader, width, height, camPos);
            int slot = 0;
            if (dependModelPass > 0 && id == fisrtID) {
                assert(ModelPasses.size() >= dependModelPass);
                slot = subpass.bindPreviousModelAttachments(shader, ModelPasses[dependModelPass - 1], slot);
            }
            if (id != fisrtID) {
                assert(ImagePasses.size() >= prevID);
                slot = subpass.bindPreviousImageAttachments(shader, ImagePasses[prevID - 1], slot);
            }
            slot = subpass.bindScreenAttachments(shader, screenTexture, screenDepthTexture, slot);
            subpass.bindPlights(shader, pLights);
            subpass.bindDlights(shader, cam_center, dLights);

            renderBackEnd->drawInQuad();

            if (subpass.hasInternalFramebuffer()) {
                subpass.internalFrameBuffer->unbind();
            }
            screenBuffer->unbind();
        }
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

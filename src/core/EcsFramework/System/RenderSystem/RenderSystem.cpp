//
// Created by ZhangHao on 2023/2/19.
//

#include "RenderSystem.h"
#include "EcsFramework/Component/ComponentI.h"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/ModelComponent.h"
#include "EcsFramework/Component/LightComponent.hpp"
#include "glm/fwd.hpp"
#include "Render/Light.h"

#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>
#include <string>
int windowWidth=1280,windowHeight=720;
unsigned int DLightNum = 2;

void SPW::RenderSystem::initial()
{

    renderBackEnd->Init();
    renderBackEnd->loadShaderLib("./resources/shaders/baselib");

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    renderBackEnd->InitSkyBox();
    std::vector<std::string> faces
    {
        "resources/texture/skybox/right.jpg",
        "resources/texture/skybox/left.jpg",
        "resources/texture/skybox/top.jpg",
        "resources/texture/skybox/bottom.jpg",
        "resources/texture/skybox/front.jpg",
        "resources/texture/skybox/back.jpg"
    };
    renderBackEnd->SetSkyBox(faces);
    renderBackEnd->creatShadowFrameBuffer(DLightNum);
    renderBackEnd->setUpShadowArray(DLightNum);
    for(int i = 0; i < DLightNum; i++)
    {
        renderBackEnd->shadowFrameBuffers[i]->genFrameBuffer();
        renderBackEnd->shadowFrameBuffers[i]->bind();
        renderBackEnd->shadowFrameBuffers[i]->AttachDepthTexture3D(renderBackEnd->depthTextureArray,i);
        renderBackEnd->shadowFrameBuffers[i]->unbind();
    }

}

void SPW::RenderSystem::beforeUpdate() {
    // clear buffer
}

void SPW::RenderSystem::onUpdate(SPW::TimeDuration dt) {

}

void SPW::RenderSystem::afterUpdate(){
    // get all normal cameras and UI cameras

    RenderCamera uiCamera;

    ComponentGroup<SPW::IDComponent,
        SPW::CameraComponent,
        SPW::TransformComponent> cameraGroup;

    ComponentGroup<SPW::IDComponent, 
                    SPW::TransformComponent, 
                    SPW::LightComponent> lightGroup;

    glm::mat4 V,P;
    locatedScene.lock()->forEachEntityInGroup(cameraGroup,
        [this, &uiCamera, &cameraGroup,&V,&P](const Entity &en){
            RenderCamera camera = en.combinedInGroup(cameraGroup);
            if (std::get<1>(camera)->getType() != UIOrthoType)
                // render models on this camera
                renderModelsWithCamera(camera,V,P);
            else
                // pick uiCamera out
                // UI should be rendered in final step
                uiCamera = camera;
        });
    V = glm::mat4(glm::mat3(V));
    renderBackEnd->drawSkyBox(V,P);

    frameBuffer->unbind();

    //RenderPass n-1, PostProcessing
    postProcessPass.pushCommand(SPW::RenderCommand(&SPW::RenderBackEndI::DepthTest, false));
    postProcessPass.pushCommand(SPW::RenderCommand(&SPW::RenderBackEndI::SetClearColor,glm::vec4(0.5)));
    postProcessPass.pushCommand(SPW::RenderCommand(&SPW::RenderBackEndI::Clear));
    postProcessPass.pushCommand(SPW::RenderCommand(&SPW::RenderBackEndI::drawInTexture,PostProcessingEffects::FXAA));
    postProcessPass.executeWithAPI(renderBackEnd);

    // RenderPass n, render in Game GUI
    //renderModelsWithCamera(uiCamera);
}


void SPW::RenderSystem::renderModelsWithCamera(const RenderCamera &camera,glm::mat4& View,glm::mat4& Pro)
{
    if (std::get<0>(camera) == nullptr)
    {
        return;
    }
    UUID currentID = std::get<0>(camera)->getID();
    auto cameraCom = std::get<1>(camera);
    auto transformCom = std::get<2>(camera);
    glm::vec3 camPos = transformCom->position;

    using ShaderModelMap = std::unordered_map<ShaderHandle, std::vector<SPW::Entity>, ShaderHash, ShaderEqual>;

    // predefine render objects
    ComponentGroup<SPW::IDComponent,
        SPW::ModelComponent,
        SPW::TransformComponent> modelGroup;

    // 1. get associated objects from camera
    std::vector<SPW::Entity> renderModels;
    locatedScene.lock()->forEachEntityInGroup(
        modelGroup,
        [&currentID, &renderModels, this](const Entity &en) {
            auto modelCom = en.component<SPW::ModelComponent>();
            // passing data to GPU
            if (modelCom->bindCameras.find(currentID) != modelCom->bindCameras.end()) {
                if (!modelCom->ready)
                {
                    modelCom->model->setUpModel(renderBackEnd);
                    modelCom->ready = true;
                }
                renderModels.push_back(en);
            }
    });
    // 2. calculate VP from camera
    glm::mat4x4 V, P;
    glm::mat4x4 cameraTransform = glm::mat4(1.0f);
    cameraTransform = glm::translate(cameraTransform, transformCom->position);
    cameraTransform = cameraTransform * glm::eulerAngleYXZ(
        glm::radians(transformCom->rotation.y),
        glm::radians(transformCom->rotation.x),
        glm::radians(transformCom->rotation.z)
    );

    glm::vec4 eye(0, 0, 0, 1), look_at(0, 0, -1, 1), up(0, 1, 0, 0);
    V = glm::lookAt(glm::vec3(cameraTransform * eye),
                    glm::vec3(cameraTransform *look_at),
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

    // get lights from scene
    ComponentGroup<SPW::IDComponent,
        SPW::LightComponent,
        SPW::TransformComponent> lightGroup;
    std::vector<PLight> pLights;
    std::vector<DLight> dLights;
    locatedScene.lock()->forEachEntityInGroup(lightGroup, [&lightGroup, &pLights, &dLights](const Entity &en){
        auto [id, light, trans] = en.combinedInGroup(lightGroup);
        if (light->getType() == PointLightType) {
            PLight pl = {};
            pl.position = trans->position;
            pl.ambient = light->ambient;
            pl.diffuse = light->diffuse;
            pl.specular = light->specular;
            pl.constant = light->constant;
            pl.linear = light->linear;
            pl.quadratic = light->quadratic;
            pLights.push_back(pl);
        } else {
            glm::vec4 dir = {0, 0, -1, 0};
            auto rotMat = glm::eulerAngleYX(
                       glm::radians(trans->rotation.y),
                       glm::radians(trans->rotation.x));
            DLight dl = {};
            dl.direction = rotMat * dir;
            dl.ambient = light->ambient;
            dl.diffuse = light->diffuse;
            dl.specular = light->specular;
            dLights.push_back(dl);
        }
    });
    
    if(cameraCom->whetherMainCam)
    {
        View = V;
        Pro = P;
    }

    // RenderPass 1, shadow
    // sort models with program, build a map with shadow_program -> models[]
    auto renderPass = [this, &renderModels, &V, &P, &camPos, &pLights, &dLights](bool isShadow){
        ShaderModelMap programModelMap;
        for (auto &en : renderModels)
        {
            // get program that used in shadow rendering
            auto program =
                isShadow ? en.component<SPW::ModelComponent>()->shadowProgram :
                en.component<SPW::ModelComponent>()->modelProgram;

            if (program.isValid()) {
                // record models in current program
                programModelMap[program].push_back(en);
            }
        }
        if(isShadow)
        {
            for (auto [handle, entities] : programModelMap)
            {
                auto shader = renderBackEnd->getShader(handle);
                shader->Bind();
                // bind light
                for (unsigned int i = 0; i < pLights.size(); i++) {
                    auto pl = pLights[i];
                    shader->setPLight(i, pl);
                }
                for (unsigned int i = 0; i < dLights.size(); i++) {
                    auto dl = dLights[i];
                    shader->setDLight(i, dl);
                }

                for(int i = 0; i < DLightNum; i++)
                {
                    renderBackEnd->shadowFrameBuffers[i]->bind();
                    glm::vec3 lightPos = -dLights[i].direction*5.0f;
                    glm::mat4 lightProjection, lightView;
                    glm::mat4 lightSpaceMatrix;
                    float near_plane = 1.0f, far_plane = 10.5f;
                    lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
                    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
                    lightSpaceMatrix = lightProjection * lightView;
                    shader->SetUniformValue<glm::mat4> ("lightSpaceMatrix", lightSpaceMatrix);
                    renderBackEnd->SetViewport(0,0,FrameBuffer::SHADOW_WIDTH, FrameBuffer::SHADOW_HEIGHT);
                    renderBackEnd->shadowFrameBuffers[i]->bind();
                    renderBackEnd->Clear();
                    // render every model in this shader
                    for (auto &model : entities)
                    {
                        auto modelCom = model.component<ModelComponent>();
                        auto transformCom = model.component<TransformComponent>();

                        glm::mat4 M = glm::mat4(1.0f);
                        M = glm::translate(M,transformCom->position);
                        M = M * glm::eulerAngleYXZ(glm::radians(transformCom->rotation.y),
                                                   glm::radians(transformCom->rotation.x),
                                                   glm::radians(transformCom->rotation.z));
                        M = glm::scale(M, transformCom->scale);

                        shader->SetUniformValue<glm::mat4>("M", M);
                        modelCom->model->Draw(renderBackEnd, handle);
                    }
                    renderBackEnd->shadowFrameBuffers[i]->unbind();
                    // reset viewport
                    renderBackEnd->SetViewport(0,0,windowWidth, windowHeight);
                    renderBackEnd->Clear();
                }
            }
        }
        else
        {
            frameBuffer->bind();
            renderBackEnd->DepthTest(true);
            renderBackEnd->SetClearColor(glm::vec4(0.5));
            renderBackEnd->Clear();
            for (auto [handle, entities] : programModelMap)
            {
                auto shader = renderBackEnd->getShader(handle);
                shader->Bind();
                shader->SetUniformValue<glm::vec3>("camPos", camPos);
                shader->SetUniformValue<glm::mat4>("V", V);
                shader->SetUniformValue<glm::mat4>("P", P);
                // bind light
                for (unsigned int i = 0; i < pLights.size(); i++) {
                    auto pl = pLights[i];
                    shader->setPLight(i, pl);
                }
                for (unsigned int i = 0; i < dLights.size(); i++) {
                    auto dl = dLights[i];
                    shader->setDLight(i, dl);
                }

                for(int i = 0; i< DLightNum; i++)
                {
                    glm::vec3 lightPos = -dLights[i].direction*5.0f;

                    glm::mat4 lightProjection, lightView;
                    glm::mat4 lightSpaceMatrix;

                    float near_plane = 1.0f, far_plane = 10.5f;
                    lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
                    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
                    lightSpaceMatrix = lightProjection * lightView;
                    shader->SetUniformValue<glm::mat4> ("lightSpaceMatrix["+std::to_string(i)+"]", lightSpaceMatrix);
                    shader->bindTexArray(5+i,renderBackEnd->depthTextureArray);
                    shader->setInt("shadowMap",5+i);

                }
                shader->SetUniformValue("PLightCount", int(pLights.size()));
                shader->SetUniformValue("DLightCount", int(dLights.size()));
                // render every model in this shader
                for (auto &model : entities)
                {
                    auto modelCom = model.component<ModelComponent>();
                    auto transformCom = model.component<TransformComponent>();

                    glm::mat4 M = glm::mat4(1.0f);
                    M = glm::translate(M,transformCom->position);
                    M = M * glm::eulerAngleYXZ(glm::radians(transformCom->rotation.y),
                                               glm::radians(transformCom->rotation.x),
                                               glm::radians(transformCom->rotation.z));
                    M = glm::scale(M, transformCom->scale);

                    shader->SetUniformValue<glm::mat4>("M", M);
                    modelCom->model->Draw(renderBackEnd, handle);
                }
            }
        }
    };

    // shadow pass
    frameBuffer->bind();
    renderBackEnd->DepthTest(true);
    renderBackEnd->SetClearColor(glm::vec4(0.5));
    renderBackEnd->Clear();
    renderPass(true);
    // model pass
    renderPass(false);
}

void SPW::RenderSystem::onStop() {

}

bool SPW::RenderSystem::onFrameResize(int w, int h) {
    std::cout << "RenderSystem frame changed" << std::endl;
    // update frame buffer here
    windowWidth = w;
    windowHeight = h;
    frameBuffer->deleteFrameBuffer();
    frameBuffer->genFrameBuffer();
    frameBuffer->bind();
    frameBuffer->AttachColorTexture(w,h,0);
    frameBuffer->AttachDepthRenderBuffer(w,h);
    frameBuffer->CheckFramebufferStatus();
    frameBuffer->unbind();
    return false;
}
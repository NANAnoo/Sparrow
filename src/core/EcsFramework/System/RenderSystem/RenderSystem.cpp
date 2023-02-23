//
// Created by ZhangHao on 2023/2/19.
//

#include "RenderSystem.h"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/ModelComponent.h"

#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>

void SPW::RenderSystem::initial() {
    renderBackEnd->Init();
}

void SPW::RenderSystem::beforeUpdate() {
    // clear buffer
    renderBackEnd->Clear();
}

void SPW::RenderSystem::onUpdate(SPW::TimeDuration dt) {

}

void SPW::RenderSystem::afterUpdate() {
    // get all normal cameras and UI cameras
    RenderCamera uiCamera;

    ComponentGroup<SPW::IDComponent,
        SPW::CameraComponent,
        SPW::TransformComponent> cameraGroup;

    locatedScene.lock()->forEachEntityInGroup(cameraGroup,
        [this, &uiCamera, &cameraGroup](const Entity &en){
            RenderCamera camera = en.combinedInGroup(cameraGroup);
            if (std::get<1>(camera)->getType() != UIOrthoType)
                // render models on this camera
                renderModelsWithCamera(camera);
            else
                // pick uiCamera out
                // UI should be rendered in final step
                uiCamera = camera;
        });

    // RenderPass n-1, PostProcessing
    postProcessPass.executeWithAPI(renderBackEnd);

    // RenderPass n, render in Game GUI
    renderModelsWithCamera(uiCamera);
}


void SPW::RenderSystem::renderModelsWithCamera(const RenderCamera &camera) {
    if (std::get<0>(camera) == nullptr) {
        return;
    }
    UUID currentID = std::get<0>(camera)->getID();
    auto cameraCom = std::get<1>(camera);
    auto transformCom = std::get<2>(camera);

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
            if (modelCom->cameraID == currentID) {
                if (!modelCom->ready) {
                    modelCom->model->setUpModel(renderBackEnd);
                }
                renderModels.push_back(en);
            }
    });

    // 2. calculate VP from camera
    glm::mat4x4 V, P;
    glm::mat4x4 cameraTransform = glm::eulerAngleYXZ(transformCom->rotation.y,
                                        transformCom->rotation.x,
                                        transformCom->rotation.z);
    glm::translate(cameraTransform, transformCom->position);
    glm::vec4 eye(0, 0, 1, 1), look_at(0, 0, 0, 1), up(0, 1, 0, 0);
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


    // RenderPass 1, shadow
    // sort models with program, build a map with shadow_program -> models[]

    auto renderPass = [this, &renderModels, &V, &P](bool isShadow){
        ShaderModelMap programModelMap;
        for (auto &en : renderModels) {
            // get program that used in shadow rendering
            auto program =
                isShadow ? en.component<SPW::ModelComponent>()->shadowProgram :
                en.component<SPW::ModelComponent>()->modelProgram;

            if (program.isValid()) {
                // record models in current program
                programModelMap[program].push_back(en);
            }
        }

        // for each program, draw every model
        for (auto [handle, entities] : programModelMap) {
            for (auto &model : entities) {
                auto modelCom = model.component<ModelComponent>();
                auto transformCom = model.component<TransformComponent>();
                glm::mat4x4 M = glm::mat4(1.0f);
                M = glm::translate(M, transformCom->position);
                M = M * glm::eulerAngleXYZ(glm::radians(transformCom->rotation.x),
                                           glm::radians(transformCom->rotation.y),
                                           glm::radians(transformCom->rotation.z));
                M = glm::scale(M, transformCom->scale);

                for(auto &mesh : modelCom->model->meshes) {
                    // set up mesh with current shader
                    mesh.setShader(renderBackEnd, handle);
                    // TODO: maybe add animation data to uniform here?

                    // set up MVP
                    mesh.shader->SetUniformValue<glm::mat4>("M", M);
                    mesh.shader->SetUniformValue<glm::mat4>("V", V);
                    mesh.shader->SetUniformValue<glm::mat4>("P", P);
                    // draw current model
                    mesh.Draw(renderBackEnd);
                }
            }
        }
    };

    // shadow pass
    renderPass(true);

    // model pass
    renderPass(false);
}

void SPW::RenderSystem::onStop() {

}
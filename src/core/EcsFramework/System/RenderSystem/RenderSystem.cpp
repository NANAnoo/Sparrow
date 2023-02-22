//
// Created by ZhangHao on 2023/2/19.
//

#include "RenderSystem.h"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/ModelComponent.h"
void SPW::RenderSystem::initial() {

}

void SPW::RenderSystem::beforeUpdate() {

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
    glm::mat4x4 VP = glm::mat4(1.0f);
    // TODO @Ding : calculate VP from a transformed camera.

    // RenderPass 1, shadow
    // sort models with program, build a map with shadow_program -> models[]

    auto renderPass = [this, &renderModels, &VP](bool isShadow){
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

                // TODO: calculate MVP here
                // TODO: MVP = m * VP;
                for(auto &mesh : modelCom->model->meshes) {
                    // set up mesh with current shader
                    mesh.setShader(renderBackEnd, handle);
                    // TODO: maybe add animation data to uniform here?

                    // TODO: set up MVP here
                    // mesh.shader->SetUniformValue<glm::mat4>(MVP);
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
//
// Created by ZhangHao on 2023/2/19.
//

#include "RenderSystem.h"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/ModelComponent.h"
void SPW::RenderSystem::initial() {

}

void SPW::RenderSystem::beforeUpdate() {
    locatedScene.lock()->forEach([this](SPW::ModelComponent *modelCom){
        if (!modelCom->ready) {
            modelCom->model->setUpModel(renderBackEnd);
            modelCom->ready = true;

            for (auto &mesh : modelCom->model->meshes) {
                mesh.setShader(modelCom->name, modelCom->vertex_shader_path, modelCom->frag_shader_path, renderBackEnd);
            }

        }
    }, SPW::ModelComponent);

    RenderCommand(&RenderBackEndI::SetClearColor, {0.1, 0, 0, 1}).execute(renderBackEnd);
    RenderCommand(&RenderBackEndI::Clear).execute(renderBackEnd);
}

void SPW::RenderSystem::onUpdate(SPW::TimeDuration dt) {

}

void SPW::RenderSystem::afterUpdate() {
    // create different pass
    locatedScene.lock()->forEach([this](SPW::ModelComponent *modelCom){
        if (modelCom->ready) {
            modelCom->model->Draw(renderBackEnd);
        }
    }, SPW::ModelComponent);
}

void SPW::RenderSystem::onStop() {

}
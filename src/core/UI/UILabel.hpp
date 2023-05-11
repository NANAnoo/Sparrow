//
// Created by 29134 on 2023/4/14.
//

#include "UIResponder.hpp"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/MeshComponent.hpp"

namespace SPW{

    class UILabel : public UIResponder{
    public:
        explicit UILabel(const std::shared_ptr<UIResponder> &p, std::shared_ptr<Scene> &scene, const char* name):
        UIResponder(std::dynamic_pointer_cast<SPW::EventResponderI>(p)), scene(scene), name_(name) {

            if(name == ""){
                std::cout << "Label creating failed! Label name can not be empty!" << std::endl;
                return;
            }

            pos_x = 0;
            pos_y = 0;
            width = 50;
            height = 50;

            depth = scene->ui_responder->rootDepth + 1;
            scene->ui_responder->rootDepth = depth;

            label = scene->createEntity(name);
            label->emplace<SPW::TransformComponent>();
            auto model = label->emplace<SPW::MeshComponent>(scene->uiCamera->getUUID());
            //model->model = SPW::createUIMesh(bg_img);
            model->bindRenderGraph = scene->getUIRenderGraphID();
            model->modelSubPassPrograms[scene->getUIRenderNodeID()] = scene->getUIProgramID();
            updateLayout();
        };

        void setSize(int width, int height){
            this->width = width;
            this->height = height;
            updateLayout();
        }

        void setPosition(int pos_x, int pos_y){
            this->pos_x = pos_x;
            this->pos_y = pos_y;
            updateLayout();
        }

        void setName(const char* name){
            name_ = name;
        }

    private:

        void updateLayout(){
            label->component<SPW::TransformComponent>()->position = {this->pos_x, this->pos_y, -depth};
            label->component<SPW::TransformComponent>()->scale = {this->width, this->height, 1};
        }

        std::shared_ptr<Scene> scene;
        std::shared_ptr<SPW::Entity> label;
        const char* name_;
        const char* bg_img = "./resources/texture/skybox/right.jpg";
    };
};
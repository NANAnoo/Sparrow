//
// Created by 29134 on 2023/4/7.
//

#include "UIResponder.hpp"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"

namespace SPW{

    class UIButton : public UIResponder{
    public:
        explicit UIButton(const std::shared_ptr<UIResponder> &p, std::shared_ptr<Scene> &scene, const char* name):
        UIResponder(std::dynamic_pointer_cast<SPW::EventResponderI>(p)), scene(scene), name_(name) {

            if(name == ""){
                std::cout << "Button creating failed! Button name can not be empty!" << std::endl;
                return;
            }

            pos_x = 0;
            pos_y = 0;
            width = 50;
            height = 50;

            depth = scene->ui_responder->rootDepth + 1;
            scene->ui_responder->rootDepth = depth;

            button = scene->createEntity(name);
            button->emplace<SPW::TransformComponent>();
            auto mesh = button->emplace<SPW::MeshComponent>(scene->uiCamera->getUUID());
            mesh->bindRenderGraph = scene->getUIRenderGraphID();
            mesh->modelSubPassPrograms[scene->getUIRenderNodeID()] = scene->getUIProgramID();
            mesh->assetID = button->getUUID().toString();
            auto builder = SPW::BasicMeshStorage<Spw::UIMesh>::getInstance().insert(mesh->assetID);
            auto material = SPW::MaterialData();
            auto id = SPW::UUID::randomUUID;
            material.m_TextureIDMap[SPW::TextureMapType::Albedo] = id.toString();
            updateLayout();
        };

        const char* getName() override{
            return "SPWButtonEvent";
        }

        void solveEvent(const std::shared_ptr<EventI> &e) override{
            e->dispatch<MouseDownType, MouseEvent>(EVENT_RESPONDER(buttonOnClick));
            e->dispatch<CursorMovementType, MouseEvent>(EVENT_RESPONDER(buttonOnHover));
        }

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

        void setButtonOnClick(std::function<void()> onClick) {
            onClick_ = onClick;
        }

        void setButtonOnHover(std::function<void()> onHover) {
            onHover_ = onHover;
        }

        void setButtonNoHover(std::function<void()> noHover) {
            noHover_ = noHover;
        }

        void setName(const char* name){
            name_ = name;
        }

        void setBackgroundImage(const char* img_path){
            bg_img = img_path;
        }

    private:
        bool buttonOnClick(MouseEvent *e){
            onClick_();
            return true;
        }

        bool buttonOnHover(MouseEvent *e){
            if(isHovered){
                double cursorX = e->cursor_xpos + e->cursor_xpos_bias;
                double cursorY = e->cursor_ypos + e->cursor_ypos_bias;
                if(cursorX < this->pos_x - bias || cursorX > this->pos_x + this->width - bias ||
                e->window_height - cursorY < this->pos_y - bias ||
                e->window_height - cursorY > this->pos_y + this->height - bias){
                    isHovered = false;
                    buttonNoHover();
                }
            }
            else{
                isHovered = true;
                if (onHover_)
                    onHover_();
                 else
                    std::cout << "onHover unset!" << std::endl;
            }
            return true;
        }

        void buttonNoHover(){
            if (noHover_)
                noHover_();
            else
                std::cout << "noHover unset!" << std::endl;
        }

        void updateLayout(){
            button->component<SPW::TransformComponent>()->position = {this->pos_x, this->pos_y, -depth};
            button->component<SPW::TransformComponent>()->scale = {this->width, this->height, 1};
        }

        std::shared_ptr<Scene> scene;
        std::shared_ptr<SPW::Entity> button;
        const char* name_;
        const char* bg_img = "./resources/texture/skybox/right.jpg";
        std::function<void()> onClick_;
        std::function<void()> onHover_;
        std::function<void()> noHover_;
        bool isHovered = false;
    };
};

#ifndef SPARROW_IMGUIWINDOW_H
#define SPARROW_IMGUIWINDOW_H

#include "ImGuiPanel.h"
#include "EcsFramework/System/NewRenderSystem/SPWRenderSystem.h"

namespace SPW{
    class ImGuiWindow : public ImGuiPanel{

    public:
        ImGuiWindow(std::string title, bool *open = nullptr)
                : ImGuiPanel(std::move(title), open)
        {}

        void DrawWindowPanel(std::shared_ptr<SPW::SPWRenderSystem> ptr){

        }
        void Draw() override{
           // DrawWindowPanel()
        }
    };
}






#endif //SPARROW_IMGUIWINDOW_H

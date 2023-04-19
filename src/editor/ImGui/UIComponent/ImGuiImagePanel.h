#include "ImGuiPanel.h"
#include "ImGui/UIComponent/ImGuiPanel.h"

namespace SPW {

	class ImGuiImagePanel : public ImGuiPanel
	{
	public:
		ImGuiImagePanel(int64_t tid)
			: ImGuiPanel("Image Panel")
			, texture_id(tid)
		{	}

	protected:

		void Begin() override
		{
			ImGui::Begin(ImGuiPanel::m_title.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar);
		}

		void Draw() override
		{
            //Starter and Loader
            //StarterAndLoader();

            //Main scene window
            ImVec2 windowSize = ImGui::GetWindowSize();
//            ImVec2 panelSize = ImVec2(windowSize.x - 20.0f,windowSize.y - 70.0f);
            ImGui::SetNextWindowSize(windowSize);
            ImGui::SetCursorPos(ImVec2(0,0));

			ImGui::Image
			(
				(void*)texture_id,
				windowSize,
 			    ImVec2(0.0, 1.0),
				ImVec2(1.0, 0.0),
				ImVec4(1, 1, 1, 1),
				ImVec4(0, 0, 0, 1.0f)
			);
		}

        void StarterAndLoader(){

            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 buttonSize(30, 30);
            ImVec2 buttonPos((windowSize.x - buttonSize.x - 100.0f) / 2, 20.0f);
            ImVec2 buttonPos2((windowSize.x - buttonSize.x - 10.0f) / 2, 20.0f);
            ImGui::SetCursorPos(buttonPos2);
            if (ImGui::Button(">", buttonSize))
            {
                if(!isPlaying)
                {
                    isPlaying = true;

                }
                //TODO::CHANGE SYSTEM
                
            }

            ImGui::SetCursorPos(buttonPos);
            if (ImGui::Button("||", buttonSize))
            {
                if(isPlaying) isPlaying = false;
                // Button was clicked!
                //TODO::CHANGE SYSTEM
            }

        }

	private:
		int64_t texture_id;
        bool isPlaying = false;
	};

}

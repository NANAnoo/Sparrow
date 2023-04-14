#include "ImGuiPanel.h"
#include "ImGui/UIComponent/ImGuiPanel.h"

namespace SPW {

	class ImGuiImagePanel : public ImGuiPanel
	{
	public:
		ImGuiImagePanel(uint32_t tid) : ImGuiPanel("Image Panel")
		{
			texture_id = tid;
		}

	protected:
		void Draw() override
		{
			ImTextureID image_id = (void*)(intptr_t)texture_id;

			//TODO
            ImVec2 panelSize_test = ImGui::GetWindowSize();
            //printf("Panel Width: %.2f, Panel Height: %.2f\n", panelSize_test.x, panelSize_test.y);
            ImVec2 panelSize = ImVec2(panelSize_test.x - 1.0f, panelSize_test.y - 1.0f);
            ImGui::SetNextWindowSize(panelSize);
            ImVec2 imageSize = panelSize;

            ImVec2 panelPos = ImVec2((ImGui::GetWindowSize().x - panelSize.x) / 2, (ImGui::GetWindowSize().y - panelSize.y) / 2);
            ImGui::SetCursorPos(panelPos);
            ImGui::Dummy(panelSize_test);

            ImVec2 imagePos = ImVec2(panelPos.x + (panelSize.x - imageSize.x) / 2, panelPos.y + (panelSize.y - imageSize.y) / 2);

            ImGui::SetCursorPos(imagePos);
            ImGui::Image(image_id, imageSize, ImVec2(0.0, 1.0), ImVec2(1.0, 0.0), ImVec4(1, 1, 1, 1), ImVec4(0, 1, 0, 1));

		}

	private:
		uint32_t texture_id;
        int windows_num = 0;
	};

}

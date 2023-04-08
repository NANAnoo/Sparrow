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
			ImVec2 panelSize = ImVec2(400, 300); // 设置面板大小
			ImVec2 imageSize = panelSize; // 图像大小等于面板大小
			ImVec2 panelPos = ImVec2((ImGui::GetWindowSize().x - panelSize.x) / 2, (ImGui::GetWindowSize().y - panelSize.y) / 2); // 计算面板位置使其居中

			ImGui::SetCursorPos(panelPos); // 设置光标位置为面板位置
			ImGui::Dummy(panelSize); // 用于设置面板的大小，使后续的控件能够居中

			ImVec2 imagePos = ImVec2(panelPos.x + (panelSize.x - imageSize.x) / 2, panelPos.y + (panelSize.y - imageSize.y) / 2);

			ImGui::SetCursorPos(imagePos); // 设置光标位置为图像位置
			ImGui::Image(image_id, imageSize, ImVec2(0.0, 1.0), ImVec2(1.0, 0.0), ImVec4(1, 1, 1, 1), ImVec4(0, 1, 0, 1));
		}

	private:
		uint32_t texture_id;
	};

}

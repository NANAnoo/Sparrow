/*
 * @date   2023-04-11
 * @author dudu
 * @brief  https://github.com/leiradel/ImGuiAl/
 */
#include "ImGuiMessageBox.h"
namespace SPW
{
	int ImGuiMessageBox::Exec()
	{
		uint32_t selected_option = -1;

		if (ImGui::BeginPopupModal(m_Title, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (m_DontAskAgain && m_Selected != 0)
			{
				ImGui::CloseCurrentPopup();
				selected_option = m_Selected;
			}
			else
			{
				if (m_Icon != nullptr)
				{
					const ImVec2 size = ImGui::CalcTextSize(m_Icon);
					ImVec2 pos = ImGui::GetCursorPos();
					const float save_y = pos.y;
					pos.y += size.y;

					ImGui::SetCursorPos(pos);
					ImGui::Text("%s", m_Icon);

					pos.x += size.x + pos.x;
					pos.y = save_y;

					ImGui::SetCursorPos(pos);
					ImGui::TextWrapped("%s", m_Text);
				}
				else
				{
					ImGui::TextWrapped("%s", m_Text);
				}

				ImGui::Separator();

				if (m_ShowCheckbox)
				{
					ImGui::Checkbox("Don't ask me again", &m_DontAskAgain);
				}

				ImVec2 size = ImVec2(50.0f, 0.0f);

				uint32_t idx = 0;
				for(const char* option : m_Captions)
				{
					ImGui::SameLine();

					idx++;
					if (ImGui::Button(option, size)) 
					{
						std::cout << "ciliked on option: " << option << std::endl;
						trigger_flag = false;
						selected_option = idx;
						ImGui::CloseCurrentPopup();
						break;
					}
				}

				size = ImVec2((4 - idx) * 50.0f, 1.0f);
				if(size.x < min_width)
				{
					size.x = min_width;
				}
				if(size.y < min_height)
				{
					size.y = min_height;
				}

				ImGui::Dummy(ImVec2(size));

				if (m_DontAskAgain)
				{
					m_Selected = selected_option;
				}
			}

			ImGui::EndPopup();
		}

		// trigger_flag = false;

		return selected_option;
	}


	void ImGuiMessageBox::AskAgain()
	{
		m_DontAskAgain = false;
		m_Selected = 0;
	}

}
